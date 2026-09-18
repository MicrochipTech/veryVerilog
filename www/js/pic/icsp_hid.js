
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
        this.progressCallback = null;
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
        return await this.readWordBlock(this.pic.getFlashSizeWords());
    }

    async readEEPROM() {
        console.log('Get contents of EEPROM: setPC 0xF000');     
        let eepromAddress = this.pic.getEEPROMAddress();
        let _eeprom = [];
        if(eepromAddress != null && this.pic.EESIZ != 0) {
            await this.setPC(this.pic.getPCAddress(eepromAddress));
            _eeprom = await this.readWordBlock(this.pic.EESIZ);    
        }
        return _eeprom;
    }

    async readUserID() {
        console.log('Get contents of UserId: setPC 0x8000');        
        let userIdAddress = this.pic.getUserIdAddress();
        await this.setPC(this.pic.getPCAddress(userIdAddress));
        return await this.readWordBlock(this.pic.getUserIdSize());
    }

    async readConfigWords() {
        console.log('Get contents of Config Words');
        let configWords = [];
        for (const address of this.pic.getConfigWordAddresses()) {
            await this.setPC(this.pic.getPCAddress(address));
            configWords.push(await this.readWord());
        }
        return configWords;
    }

    async readDevice() {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();

        const totalSteps = 6;
        let currentStep = 0;

        if (this.progressCallback) this.progressCallback(++currentStep / totalSteps, 'Reading Flash...');
        let memory = await this.readFlash();

        if (this.progressCallback) this.progressCallback(++currentStep / totalSteps, 'Reading EEPROM...');
        let eepromAddress = this.pic.getEEPROMAddress();
        let eeprom = await this.readEEPROM();

        if (this.progressCallback) this.progressCallback(++currentStep / totalSteps, 'Reading UserID...');
        let userIdAddress = this.pic.getUserIdAddress();
        let userId = await this.readUserID();

        if (this.progressCallback) this.progressCallback(++currentStep / totalSteps, 'Reading Config Words...');
        let configWordsAddress = this.pic.getConfigWordsAddress();
        let configWords = await this.readConfigWords();

        if (this.progressCallback) this.progressCallback(++currentStep / totalSteps, 'Exiting LVP...');
        console.log('lvpExit');
        await this.lvpExit();

        if (this.progressCallback) this.progressCallback(1, 'Complete');

        return {
            "memory": memory, "eeprom": eeprom, "userId": userId, "configWords": configWords,
            "memoryAddress": 0, "eepromAddress": eepromAddress,
            "userIdAddress": userIdAddress, "configWordsAddress": configWordsAddress
        };
    }

    async writeFlash(hexObject, verify = false) {
        let verify_ok = true;
        let buffer = [];
        const flashLoopSize = this.pic.getFlashLoopSize();
        const rowStep = this.pic.getFlashRowStep();
        const wordMask = this.pic.getWordMask();
        let waitTime = this.pic.getTpIntDelayMs() * 1000;
        const cmdBuilder = this.getCommandBytes.bind(this);

        for (let pc = 0x0000; pc < flashLoopSize; pc += rowStep) {
            let row = hexObject.slicePad(this.pic.hexOffsetForFlashPC(pc), this.pic.ERSIZ * 2);
            let row16 = new Uint16Array(row.buffer);
            if (row16.every(v => v === 0xFFFF))
                continue;

            if (verify) {
                await this.setPC(pc << 1);
                let programmed_row16 = await this.readWordBlock(this.pic.ERSIZ);
                if (programmed_row16.every((value, index) => value === (row16[index] & wordMask)))
                    continue;
                const mismatchIndex = programmed_row16.findIndex((value, index) =>
                    value !== (row16[index] & wordMask));
                console.warn(`Flash verify mismatch at PC 0x${pc.toString(16)}, word ${mismatchIndex}: expected 0x${(row16[mismatchIndex] & wordMask).toString(16)}, got 0x${programmed_row16[mismatchIndex].toString(16)}`);
                verify_ok = false;
                continue;
            }

            buffer.push(...this.getCommandBytes(0x80, pc << 1));
            buffer.push(...this.pic.buildFlashRowCmds(row16, cmdBuilder, this.dataBits, waitTime));
        }
        await this.xchgCommandBlock(buffer);
        return verify_ok;
    }

    async writeEEPROM(hexObject, verify = false) {
        let verify_ok = true;
        let buffer = [];
        let eepromSize = this.pic.EESIZ;
        if (eepromSize === 0) return true;
        eepromSize += this.pic.getEEPROMAddress();
        let waitTime = this.pic.getEEPROMWriteWaitMs() * 1000;
        const cmdBuilder = this.getCommandBytes.bind(this);

        for (let pc = this.pic.getEEPROMAddress(); pc < eepromSize; pc++) {
            let data = this.pic.extractEEPROMData(hexObject, pc);
            if (data === 0xFF) continue;

            if (verify) {
                await this.setPC(pc << 1);
                let programmed_data = await this.readWordBlock(1);
                if (programmed_data[0] === data)
                    continue;
                console.warn(`EEPROM verify mismatch at 0x${pc.toString(16)}: expected 0x${data.toString(16).padStart(2, '0')}, got 0x${programmed_data[0].toString(16)}`);
                verify_ok = false;
                continue;
            }

            buffer.push(...this.getCommandBytes(0x80, pc << 1));
            buffer.push(...this.pic.buildEEPROMWriteCmd(data, cmdBuilder, this.dataBits, waitTime));
        }
        await this.xchgCommandBlock(buffer);
        return verify_ok;
    }

    async writeUserId(hexObject, verify = false) {
        let verify_ok = true;
        let buffer = [];
        let userId = [];
        let userIdSize = this.pic.getUserIdSize();
        let waitTime = this.pic.getTpIntDelayMs() * 1000;
        const emptyWord = this.pic.getEmptyWord();
        const cmdBuilder = this.getCommandBytes.bind(this);

        for (let index = 0; index < userIdSize; index++) {
            let pc = this.pic.getUserIdIterationPC(index);
            let data = this.pic.extractUserIdData(hexObject, pc);
            userId.push(data);
            if (data === emptyWord) continue;

            if (verify) {
                await this.setPC(pc << 1);
                let programmed_userId = await this.readWordBlock(1);
                if (programmed_userId[0] === data)
                    continue;
                console.warn(`User ID verify mismatch at 0x${pc.toString(16)}: expected 0x${data.toString(16)}, got 0x${programmed_userId[0].toString(16)}`);
                verify_ok = false;
                continue;
            }

            buffer.push(...this.getCommandBytes(0x80, pc << 1));
            buffer.push(...this.pic.buildUserIdWriteCmd(data, cmdBuilder, this.dataBits, waitTime));
        }
        await this.xchgCommandBlock(buffer);
        this.readUserIdFields(userId);
        return verify_ok;
    }

    async writeConfigWord(hexObject, verify = false) {
        let verify_ok = true;
        let waitTime = this.pic.getTpIntConfWordDelayMs() * 1000 * 5;
        const emptyConfig = this.pic.getEmptyConfigWord();
        const cmdBuilder = this.getCommandBytes.bind(this);

        for (let pc of this.pic.getConfigWordAddresses()) {
            let data = this.pic.extractConfigData(hexObject, pc);
            if (data === emptyConfig) continue;

            if (verify) {
                await this.setPC(pc << 1);
                let programmed_confWord = await this.readWordBlock(1);
                if (programmed_confWord[0] === data)
                    continue;
                console.warn(`Config verify mismatch at 0x${pc.toString(16)}: expected 0x${data.toString(16)}, got 0x${programmed_confWord[0].toString(16)}`);
                verify_ok = false;
                continue;
            }

            let buffer = this.getCommandBytes(0x80, pc << 1);
            buffer.push(...this.pic.buildConfigWriteCmd(data, cmdBuilder, this.dataBits, waitTime));
            await this.xchgCommandBlock(buffer, 2000);
        }
        return verify_ok;
    }

    async verifyFlashedData(trials, label, verifyFunction, hexObject) {
        console.log(`Verifying ${label}...`);
        for (let i = 0; i < trials; i++) {
            if (await verifyFunction.call(this, hexObject, true)) {
                return;
            }
        }
        throw new Error(`${label} verification failed`);
    }

    /*
     *  hexObject is an object of type MemoryMap, intel-hex.js
     */
    async programEntireDevice(hexObject, flash = true, eeprom = true, userid = true, config = true) {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();
        const trials = 2;

        const operations = [
            { enabled: flash, label: 'Flash', writeMethod: this.writeFlash },
            { enabled: eeprom, label: 'EEPROM', writeMethod: this.writeEEPROM },
            { enabled: userid, label: 'UserID', writeMethod: this.writeUserId },
            { enabled: config, label: 'Config bits', writeMethod: this.writeConfigWord }
        ];

        const enabledOps = operations.filter(op => op.enabled);
        const totalOps = enabledOps.length * (this.verify ? 2 : 1) + 1; // +1 for exit
        let currentOp = 0;

        for (const { enabled, label, writeMethod } of operations) {
            if (enabled) {
                console.log(`Writing ${label}...`);
                if (this.progressCallback) {
                    this.progressCallback(currentOp / totalOps, `Writing ${label}...`);
                }
                await writeMethod.call(this, hexObject, false);
                currentOp++;

                if (this.verify) {
                    if (this.progressCallback) {
                        this.progressCallback(currentOp / totalOps, `Verifying ${label}...`);
                    }
                    await this.verifyFlashedData(trials, label, writeMethod, hexObject);
                    currentOp++;
                }
            }
        }

        if (this.progressCallback) {
            this.progressCallback(currentOp / totalOps, 'Exiting LVP...');
        }
        console.log('lvpExit');
        await this.lvpExit();

        if (this.progressCallback) {
            this.progressCallback(1, 'Complete');
        }
    }

    readUserIdFields(userId) {
        const hex = userId.map(w => w.toString(16).toUpperCase().padStart(4, '0'));
        this.pic.userId = hex.join('.');
        if (hex.length > 8) {
            this.pic.userIdShort = hex.slice(0, 8).join('.') + '…';
        } else {
            this.pic.userIdShort = this.pic.userId;
        }
    }

    async readDeviceId() {
        // Use the HID bridge read meta-command to execute the target's Read Device ID instruction.
        // These commands return 0xA5A5 when ICSP/debug are not locked. In that case
        // the device ID must be read from its memory-mapped address instead.
        let reply = await this.xchgCommandBlock(
            this.getCommandBytes(8, 0x24, true)
        );
        if (reply.length > 0) reply = reply[0];
        // The 16-bit device ID is embedded in bits [22:7] of the 24-bit payload (start/stop bits stripped)
        let raw = (reply[1] + (reply[2] << 8) + (reply[3] << 16));
        let devID = (raw >> 1) & 0xFFFF;
        if (PIC18FQ35.deviceIdMap[devID] != null) return devID;
        return null; // use the memory-mapped ID path
    }

    async readRevisionId() {
        let reply = await this.xchgCommandBlock(
            this.getCommandBytes(8, 0x28, true)
        );
        if (reply.length > 0) reply = reply[0];
        let raw = (reply[1] + (reply[2] << 8) + (reply[3] << 16));
        return (raw >> 1) & 0xFFFF;
    }

    async getConnectionInfo() {
        console.log('lvpExit');
        await this.lvpExit();
        console.log('lvpEnter');
        await this.lvpEnter();

        // The Q35 dedicated command returns 0xA5A5 unless ICSP/debug are locked.
        // Fall back to the memory-mapped ID address for normal unlocked devices.
        let devID = await this.readDeviceId();
        let usedDirectCmd = (devID !== null);

        if (!usedDirectCmd) {
            // Probe the Q35 memory-mapped ID first. This is the normal Q35 path;
            // older families fall through to their configured ID address.
            const q35DeviceId = 0x3FFFFE;
            console.log('GetDeviceID: setPC 0x' + q35DeviceId.toString(16));
            await this.setPC(this.pic.getPCAddress(q35DeviceId));
            const q35DevID = await this.readWord();
            if (PIC18FQ35.deviceIdMap[q35DevID] != null) {
                devID = q35DevID;
            } else {
                let devIDaddress = this.pic.getDeviceIdAddress();
                console.log('GetDeviceID: setPC 0x' + devIDaddress.toString(16));
                await this.setPC(this.pic.getPCAddress(devIDaddress));
                devID = await this.readWord();
            }
        }

        let devIDx = '0x' + devID.toString(16).toUpperCase();
        console.log(`DEVID=${devIDx}`);

        this.pic = GenericPIC.getPicByDevId(devID);
        this.pic.devIDx = devIDx;

        let revIDaddress = this.pic.getRevisionIdAddress();
        if (usedDirectCmd && this.pic.hasDirectDeviceIdCmd()) {
            console.log('GetRevID: dedicated command 0x28');
            this.pic.revID = await this.readRevisionId();
        } else {
            console.log('GetRevID: setPC 0x' + revIDaddress.toString(16));
            await this.setPC(this.pic.getPCAddress(revIDaddress));
            this.pic.revID = await this.readWord();
        }
        this.pic.revIDx = '0x' + this.pic.revID.toString(16).toUpperCase();
        console.log(`REVID=${this.pic.revIDx}`);
        await this.setPC(this.pic.getPCAddress(this.pic.getDiaAddress()));
        this.pic.readDiaFields(await this.readWordBlock(this.pic.getDiaSize()));
        await this.setPC(this.pic.getPCAddress(this.pic.getDciAddress()));
        this.pic.readDciFields(await this.readWordBlock(this.pic.getDciSize()));
        await this.setPC(this.pic.getPCAddress(this.pic.getUserIdAddress()));
        this.readUserIdFields(await this.readWordBlock(this.pic.getUserIdSize()));
        console.log(`UserId=${this.pic.userId}`);
        console.log('Get contents of FLASH init: setPC 0x0000');        
        await this.setPC(0x0000);
        let first_word = await this.readWord();
        console.log(`@0x0000=0x${first_word.toString(16).toUpperCase()}`);
        console.log('lvpExit');
        await this.lvpExit();
        //console.log(`HW_baud_rate=${await this.getHwBaudRate()}`);
        //console.log(`set_baud_rate=${await this.setHwBaudRate(9600)}`);
        //console.log(`HW_baud_rate=${await this.getHwBaudRate()}`);
    }

    async getHwBaudRate() {
        let reply = await this.xchgCommandBlock(
            this.getCommandBytes(9, 0, true)
        );
        reply = reply[0];
        if (reply[0] === this.metaCmd) {
            return reply[1] + (reply[2] << 8) + (reply[3] << 16);
        } else {
            throw new Error('Unexpected reply on command "getHwBaudRate"');
        }
    }

    async setHwBaudRate(newBaud) {
        let reply = await this.xchgCommandBlock(
            this.getCommandBytes(1, Math.floor(newBaud / 100), true)
                .concat(this.getCommandBytes(9, 0, true))
        );
        reply = reply[0];
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

    setProgressCallback(callback) {
        this.progressCallback = callback;
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
            return ((reply[1] + (reply[2] << 8) + (reply[3] << 16)) >> 1) & 0xFFFF;
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
                    words.push(((reply[i+1] + (reply[i+2] << 8) + (reply[i+3] << 16)) >> 1) & 0xFFFF);
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
