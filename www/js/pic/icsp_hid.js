
/********** ICSP_HID 
 * 
 * This class implements the communication over HID 
 * to the ICSP interface on miniFPGA
 * 
 */
class ICSP_HID {

    constructor() {
        this.hid = null;
        this.pic = new GenericPIC();
        this.responsePromise = null;
        this.responseResolve = null;

        this.metaCmd = 255;
        this.cmdBits = 8;
        this.dataBits = 24;

        this.verify = false;
    }

    /*** public methods ***/

    async connect() {
        try {
            const devices = await navigator.hid.requestDevice({filters: [ {vendorId: 0x04d8} ]});
            if (devices.length === 0) {
                console.log('No HID device detected.');
                return false;
            }
            this.hid = devices[0];
            if(!this.hid.opened) {
                await this.hid.open();
                console.log('HID device connected.');
            }
            this.hid.addEventListener('inputreport', this.handleInputReport.bind(this));
            return true;
        } catch (error) {
            this.hid = null;
            console.error('Failed to connect to HID device:', error);
        }
    }

    async disconnect() {
        if (!this.hid) {
            throw new Error('HID device not connected.');
        }
        try {
            // ensure we exit lvp before disconnecting
            await this.lvpExit();
            await this.hid.close();
            this.hid = null;
            console.log('HID device disconnected.');
        } catch (error) {
            console.error('Failed to disconnect HID device:', error);
        }
    }

    async eraseDevice(flash=true, eeprom=true, userid=true, config=true) {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();
        // erase flash, eeprom, userid, config
        let param = this.pic.getEraseBits(flash, eeprom, userid, config);
        let bulkEraseTime = this.pic.getBulkEraseTimeMs() * 1000 * 2;
        console.log("Bulk erasing device...");
        let ret = await this.xchgCommandBlock(
            // bulk erase all areas
            this.getCommandBytes(0x18, param << 1)
                // wait time after bulk erase
                .concat(this.getCommandBytes(7, bulkEraseTime, true))
        );
        console.log(ret);
        let result = (ret[0][4] == 255 && ret[0][5] == 7);      
        console.log('lvpExit');
        await this.lvpExit();
        return result;
    }

    async readFlash(){
        console.log('Get contents of FLASH: setPC 0x0000');        
        await this.setPC(0x0000);
        return await this.readWordBlock(this.pic.WLSIZ * this.pic.URSIZ);
    }

    async readEEPROM() {
        console.log('Get contents of EEPROM: setPC 0xF000');     
        let eepromAddress = this.pic.getEEPROMAddress();
        let _eeprom = [];
        if(eepromAddress != null && this.pic.EESIZ != 0) {
            await this.setPC(eepromAddress * 2);
            _eeprom = await this.readWordBlock(this.pic.EESIZ);    
        }
        return _eeprom;
    }

    async readUserID() {
        console.log('Get contents of UserId: setPC 0x8000');        
        let userIdAddress = this.pic.getUserIdAddress();
        await this.setPC(userIdAddress * 2);
        return await this.readWordBlock(4);
    }

    async readConfigWords() {
        console.log('Get contents of Config Words: setPC 0x8000');        
        let configWordsAddress = this.pic.getConfigWordsAddress();
        await this.setPC(configWordsAddress * 2);
        return await this.readWordBlock(this.pic.getConfigWordsSize());
    }

    async readDevice() {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();

        let memory = await this.readFlash();
        let eepromAddress = this.pic.getEEPROMAddress();
        let eeprom = await this.readEEPROM();
        let userIdAddress = this.pic.getUserIdAddress();
        let userId = await this.readUserID();
        let configWordsAddress = this.pic.getConfigWordsAddress();
        let configWords = await this.readConfigWords();        

        console.log('lvpExit');
        await this.lvpExit();

        return {
            "memory": memory, "eeprom": eeprom, "userId": userId, "configWords": configWords,
            "memoryAddress": 0, "eepromAddress": eepromAddress, 
            "userIdAddress": userIdAddress, "configWordsAddress": configWordsAddress
        };
    }

    async writeFlash(hexObject, verify = false) {
        // this function considers that the memory is already erased        
        let verify_ok = true;
        let buffer = [];
        let flashSize = this.pic.ERSIZ * this.pic.URSIZ; // erasable row size * number of user erasable rows.
        let checkEmpty = arr => arr.every(v => v === 0xFFFF);
        let waitTime = this.pic.getTpIntDelayMs() * 1000;
        for (let pc = 0x0000; pc < flashSize; pc += this.pic.ERSIZ){
            let row = hexObject.slicePad(pc * 2, this.pic.ERSIZ * 2);
            let row16 = new Uint16Array(row.buffer);
            // check if row is empty, it it is empty, we don't need to program it.
            if(checkEmpty(row16)) continue; 
            // check if the existing row at flash is the same row to be programmed, if yes skip it,
            // otherwise include the row to be programmed into the buffer.
            // we consider that the flash is already programmed.
            if(verify) {
                await this.setPC(pc << 1);
                let programmed_row16 = await this.readWordBlock(this.pic.ERSIZ);
                if(programmed_row16.every((value, index) => value === (row16[index] & 0x3FFF)))                     
                   continue;
                verify_ok = false;
            }
            // set new PC before loading data
            buffer.push(...this.getCommandBytes(0x80, pc << 1)); 
            // load data into one row in NVM
            for(let i = 0; i < this.pic.ERSIZ; i++) {
                // load Data for NVM and increment PC. Do not increment PC if it is the last one
                buffer.push(...this.getCommandBytes(i==(this.pic.ERSIZ-1) ? 0x00:0x02, row16[i] << 1));    
            }
            // Fill the buffer with the procedures to flash the this row
            // Set payload to 0 bits
            buffer.push(...this.getCommandBytes(4, 0, true));
            // begin internally timed programming
            buffer.push(...this.getCommandBytes(0xE0, 0x00));
            // Set payload to 24 bits
            buffer.push(...this.getCommandBytes(4, this.dataBits, true));
            // wait for command to complete
            buffer.push(...this.getCommandBytes(7, waitTime, true));
        }
        await this.xchgCommandBlock(buffer);
        return verify_ok;    
    }

    async writeEEPROM(hexObject, verify = false) {
        // this function considers that the memory is already erased        
        let verify_ok = true;
        let buffer = [];
        let eepromSize = this.pic.EESIZ; 
        if(eepromSize === 0) return;
        eepromSize += this.pic.getEEPROMAddress();
        let waitTime = this.pic.getTpIntDelayMs() * 1000 * 2; // less than 2 will store wrong data
        for (let pc = this.pic.getEEPROMAddress(); pc < eepromSize; pc++){
            let eepromWord = hexObject.slicePad(pc * 2, 2);
            let data = eepromWord[0];
            // check if row is empty
            if(data == 0xFF) continue; 
            // check if the existing data at EEPROM is the same data to be programmed, if yes skip it,
            // otherwise include the row to be programmed into the buffer.
            // we consider that the flash is already programmed.
            if(verify) {
                await this.setPC(pc << 1);
                let programmed_data = await this.readWordBlock(1);
                if(programmed_data === data)                     
                   continue;
                verify_ok = false;
            }
            // set new PC before loading data
            buffer.push(...this.getCommandBytes(0x80, pc << 1)); 
            // load data into one byte of EEPROM address
            buffer.push(...this.getCommandBytes(0x00, data << 1));    
            // Set payload to 0 bits
            buffer.push(...this.getCommandBytes(4, 0, true));
            // begin internally timed programming
            buffer.push(...this.getCommandBytes(0xE0, 0x00));
            // Set payload to 24 bits
            buffer.push(...this.getCommandBytes(4, this.dataBits, true));
            // wait for command to complete
            buffer.push(...this.getCommandBytes(7, waitTime, true));
        }
        await this.xchgCommandBlock(buffer);
        return verify_ok;
    }

    async writeUserId(hexObject) {
        let buffer = [];
        let userId = [];
        let userIdSize = this.pic.getUserIdAddress() + 4; // userID: four 14 bits words.
        let waitTime = this.pic.getTpIntDelayMs() * 1000;
        let checkEmpty = arr => arr.every(v => v === 0xFF);
        for (let pc = this.pic.getUserIdAddress(); pc < userIdSize; pc++){
            let userIdx = hexObject.slicePad(pc * 2, 2);
            // check if row is empty
            if(checkEmpty(userIdx)) {
                userId.push(0x3FFF);
                continue; 
            } 
            // set new PC before loading data
            buffer.push(...this.getCommandBytes(0x80, pc << 1)); 
            // load data into one word of userID area
            let data = userIdx[0] + (userIdx[1] << 8);
            userId.push(data);
            buffer.push(...this.getCommandBytes(0x00, data << 1));    
            // Set payload to 0 bits
            buffer.push(...this.getCommandBytes(4, 0, true));
            // begin internally timed programming
            buffer.push(...this.getCommandBytes(0xE0, 0x00));
            // Set payload to 24 bits
            buffer.push(...this.getCommandBytes(4, this.dataBits, true));
            // wait for command to complete
            buffer.push(...this.getCommandBytes(7, waitTime, true));
        }
        await this.xchgCommandBlock(buffer);
        return userId;
    }

    async writeConfigWord(hexObject) {
        let waitTime = this.pic.getTpIntConfWordDelayMs() * 1000 * 5;
        for (let idx = 0; idx < this.pic.getConfigWordsSize(); idx++) {
            let buffer = [];
            let pc = this.pic.getConfigWordsAddress() + idx;
            let confWord = hexObject.slicePad(pc * 2, 2);
            let data = confWord[0] + (confWord[1] << 8);
            if((data & 0x3FFF) === 0x3FFF) continue;
            // load PC with config word address
            buffer = this.getCommandBytes(0x80, pc << 1);
            // load NVM with config word, do not increase PC
            buffer.push(...this.getCommandBytes(0x00, data << 1));
            // Set payload to 0 bits
            buffer.push(...this.getCommandBytes(4, 0, true));
            // begin internally timed programming
            buffer.push(...this.getCommandBytes(0xE0, 0x00));
            // Set payload to 24 bits
            buffer.push(...this.getCommandBytes(4, this.dataBits, true));
            // wait for command to complete
            buffer.push(...this.getCommandBytes(7, waitTime, true));
            await this.xchgCommandBlock(buffer);
        }
    }

    async verifyFlash(trials){
        console.log('Verifying Flash...');
        while(trials > 0) {
            trials--;
            if(await this.writeFlash(hexObject, true)) return;
        }
        throw new Error('Flash verification failed');
    }

    /*
     *  hexObject is an object of type MemoryMap, intel-hex.js
     */
    async programEntireDevice(hexObject, flash=true, eeprom=true, userid=true, config=true){
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();
        let trials = 2;

        if(flash) {
            console.log('Writing Flash...');     
            await this.writeFlash(hexObject, false);
            if(this.verify) {
                await this.verifyFlash(trials);
            }
        }
        if(eeprom) {
            console.log('Writing EEPROM...');     
            await this.writeEEPROM(hexObject, false);
            if(this.verify) {
                await this.verifyEEPROM(trials);
            }
        }
        if(userid) {
            console.log('Writing UserID...');   
            let userId = await this.writeUserId(hexObject);
            this.readUserIdFields(userId);
        }
        if(config) {
            console.log('Writing Config bits...');   
            await this.writeConfigWord(hexObject);
        }
        if(verify) {
            // check memory for wrong data stored
        }
        console.log('lvpExit');
        await this.lvpExit();
        return true;
    }

    readUserIdFields(userId){
        this.pic.userId = userId.map(byte => byte.toString(16).toUpperCase().padStart(4, '0')).join('.');
    }

    async getConnectionInfo() {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();
        let devIDaddress = this.pic.getDeviceIdAddress();
        console.log('GetDeviceID: setPC 0x' + devIDaddress.toString(16));
        await this.setPC(devIDaddress * 2);
        let devID = await this.readWord();
        let devIDx = '0x' + devID.toString(16).toUpperCase();
        console.log(`DEVID=${devIDx}`);

        this.pic = GenericPIC.getPicByDevId(devID);
        this.pic.devIDx = devIDx;

        let revIDaddress = this.pic.getRevisionIdAddress();
        console.log('GetRevID: setPC 0x' + revIDaddress.toString(16));
        await this.setPC(revIDaddress * 2);
        this.pic.revID = await this.readWord();
        this.pic.revIDx = '0x' + this.pic.revID.toString(16).toUpperCase();
        console.log(`REVID=${this.pic.revIDx}`);
        await this.setPC(this.pic.getDiaAddress() * 2);
        this.pic.readDiaFields(await this.readWordBlock(this.pic.getDiaSize()));
        await this.setPC(this.pic.getDciAddress() * 2);
        this.pic.readDciFields(await this.readWordBlock(this.pic.getDciSize()));
        await this.setPC(this.pic.getUserIdAddress() * 2);
        this.readUserIdFields(await this.readWordBlock(4));
        console.log(`UserId=${this.pic.userId}`);
        console.log('Get contents of FLASH init: setPC 0x0000');        
        await this.setPC(0x0000);
        let first_word = await this.readWord();
        console.log(`@0x0000=0x${first_word.toString(16).toUpperCase()}`);
        console.log('lvpExit');
        await this.lvpExit();
        //console.log(`HW_baud_rate=${await this.getHwBaudRate()}`);
        //console.log(`set_baud_rate=${await this.setHwBaudRate(115200)}`);
        //console.log(`HW_baud_rate=${await this.getHwBaudRate()}`);
    }

    async getHwBaudRate() {
        let reply = this.xchgCommandBlock(
            this.getCommandBytes(9, 0, true)
        )[0];
        if (reply[0] === this.metaCmd) {
            return reply[1] + (reply[2] << 8) + (reply[3] << 16);
        } else {
            throw new Error('Unexpected reply on command "getHwBaudRate"');
        }
    }

    async setHwBaudRate(newBaud) {
        let reply = this.xchgCommandBlock(
            this.getCommandBytes(1, Math.floor(newBaud / 100), true)
                .concat(this.getCommandBytes(9, 0, true))
        );
        if (reply[4] === this.metaCmd) {
            return reply[5] + (reply[6] << 8) + (reply[7] << 16);
        } else {
            throw new Error('Unexpected read command in "setHwBaudRate"');
        }
    }

    cdcLoopback(data) {
        let cmdList = [];
        for (let ch of data) {
            cmdList = cmdList.concat(this.getCommandBytes(11, ch, true));
        }
        let reply = this.xchgCommandBlock(cmdList);
    }

    async blinkLeds() {
        console.log('Blinking Leds...');
        let buffer = [];
        for (let idx = 0; idx < 4; idx++) {
            // GPIO cmd
            buffer.push(...this.getCommandBytes(12, 0b1000000011000000, true));
            // Delay 200 ms
            buffer.push(...this.getCommandBytes(13, 200, true));
            // GPIO cmd
            buffer.push(...this.getCommandBytes(12, 0b0100000011000000, true));
            // Delay 200 ms
            buffer.push(...this.getCommandBytes(13, 200, true));
        } 
        // GPIO cmd -> all to default 
        buffer.push(...this.getCommandBytes(12, 0, true));
        await this.xchgCommandBlock(buffer, 3000);
    }

    setVerify(state) {
        this.verify = state;
    }

    isVerify() {
        return this.verify;
    }

    /*** private methods ***/

    // receive a command block and split it into multiple data blocks if needed.
    // if the data block does not contain enough data, fullfil with nop instructions.
    // timeout is respective to each datablock transmitted.
    async xchgCommandBlock(cmdBlock, timeout = 1000) {
        // command block should be multiple of 4
        if (cmdBlock.length % 4 !== 0) {
            throw new Error('HID Command block misalignment in "xchgCommandBlock"');
        }
        let reply = [];
        let chunks = this.makeChunks(cmdBlock, 64);
        try {
            for (let cmdBlockChunk of chunks) {
                reply.push(await this.xchgData(cmdBlockChunk, 0, timeout));
            }
            return reply;
        } catch (error) {
            console.error('Error:', error.message);
        }
    }

    // exchange one block of data
    async xchgData(data, reportId = 0, timeout = 1000) {
        if (!this.hid) {
            throw new Error('HID device not connected.');
        }

        // Clear any existing timeout if a previous xchgData call is still pending
        if (this.timeoutId) {
            clearTimeout(this.timeoutId);
        }

        try {
            await this.hid.sendReport(reportId, data);
            console.log('Report sent:', reportId, data);

            // Wait for a response and wait for a promise resolution
            this.responsePromise = new Promise((resolve, reject) => {
                this.responseResolve = resolve;
                this.timeoutId = setTimeout(() => {
                    reject(new Error('Response timeout'));
                }, timeout);
            });

            // Wait for the response promise to resolve
            const response = await this.responsePromise;

            // Clear the timeout after resolving
            clearTimeout(this.timeoutId);
            this.timeoutId = null; // Reset the timeout ID

            return response;

        } catch (error) {
            console.error('Failed to send report:', error);
        }
    }

    handleInputReport(event) {
        const { data, reportId } = event;
        const receivedData = new Uint8Array(data.buffer);
        console.log('Report received:', reportId, receivedData);

        // Resolve the response promise with the received data
        if (this.responseResolve) {
            this.responseResolve(receivedData);
            this.responseResolve = null;
        }
    }

    // create an array of bytes based on a command and meta-command
    getCommandBytes(cmd, param = 0, metaCmd = false) {
        cmd = cmd & 0xFF;
        param = param & 0xFFFFFF;
        if (metaCmd) {
            param = param & 0xFFFF;
            return [this.metaCmd, cmd, param & 0xFF, (param >> 8) & 0xFF];
        } else {
            return [cmd, param & 0xFF, (param >> 8) & 0xFF, (param >> 16) & 0xFF];
        }
    }

    getNOPcmd() {
        return this.getCommandBytes(0, 0, true);
    }

    async lvpEnter() {
        return await this.xchgCommandBlock(
            this.getCommandBytes(2, this.metaCmd, true)
                .concat(this.getCommandBytes(3, this.cmdBits, true))
                .concat(this.getCommandBytes(4, this.dataBits, true))
                .concat(this.getCommandBytes(5, 0, true))
        );
    }

    async lvpExit() {
        return await this.xchgCommandBlock(
            this.getCommandBytes(6, 0, true)
        );
    }

    async setPC(newPC) {
        return await this.xchgCommandBlock(
            this.getCommandBytes(0x80, newPC)
        );
    }

    // read one word at current PC location.
    // if advancePC is true, incremente PC after read.
    async readWord(advancePC = false) {
        let reply = await this.xchgCommandBlock(
            this.getCommandBytes(8, advancePC ? 0xFE : 0xFC, true)
        );
        if(reply.length > 0) reply = reply[0];
        if (reply[0] === 0xFC) {
            return (reply[1] + (reply[2] << 8) + (reply[3] << 16)) >> 1;
        } else {
            throw new Error('Unexpected reply on command "readWord", expected 0xFC got 0x' + reply[0].toString(16).toUpperCase());
        }
    }

    // flat an array of arrays into a single array of uint8
    flattenUint8Arrays(arrays) {
        const totalLength = arrays.reduce((acc, arr) => acc + arr.length, 0);
        const flattenedArray = new Uint8Array(totalLength);
        let offset = 0;
        for (const arr of arrays) {
          flattenedArray.set(arr, offset);
          offset += arr.length;
        }
        return flattenedArray;
    }

    // read multiple 'length' words startint at current PC address
    async readWordBlock(length = 1) {
        let commandBlock = [];
        for(let i=0; i<length; i++) {
            commandBlock.push(this.getCommandBytes(8, 0xFE, true)) // read, advancePC, metacommand
        }
        let reply = await this.xchgCommandBlock(commandBlock.flat());
        reply = this.flattenUint8Arrays(reply);
        let words = [];
        if(reply.length > 0 && reply.length % 4 === 0) {
            for(let i=0; i < reply.length; i += 4) {
                if (reply[i] === 0xFF && reply[i+1] == 0x00) continue; // NOP
                else if (reply[i] === 0xFE) {
                    words.push((reply[i+1] + (reply[i+2] << 8) + (reply[i+3] << 16)) >> 1);
                } else {
                    throw new Error('Unexpected reply on command "readWord", expected 0xFC got 0x' + reply[0].toString(16).to);
                }    
            }
        } else {
            throw new Error(`Wrong buffer length received, expected ${length}, got ${reply.length / 4}`);
        }
        return words.flat();
    }

    /*  
        Function makeChunks get an array 'inputArray' (multiple of 4) and breaks this array into chunks of 'bytesPerBlock' bytes.
        This function return an array of chunks read to be transmitted as a report to the HID device.
        If there are remaing bytes available in a chunk, these bytes are filled with a filling pattern, in this case 'getNOPcmd()' 
        which is also multiple of 4. 
    */
    makeChunks(inputArray, bytesPerBlock) {
        const chunks = [];
        const fillingPattern = this.getNOPcmd();
    
        for (let i = 0; i < inputArray.length; i += bytesPerBlock) {
            const chunk = inputArray.slice(i, i + bytesPerBlock);
            const uint8Chunk = new Uint8Array(bytesPerBlock);
            uint8Chunk.set(chunk);
    
            // Fill the remaining bytes with the pattern
            for (let j = chunk.length; j < bytesPerBlock; j++) {
                uint8Chunk[j] = fillingPattern[(j - chunk.length) % fillingPattern.length];
            }    
            chunks.push(uint8Chunk);
        }    
        return chunks;
    }
}
