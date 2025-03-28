
class GenericPIC {

    static knownPics = [];
    static deviceIdMap = { 0x0000: "Unknown PIC" }

    constructor() {
        this.devID = null;
        this.revID = null;
        this.userId = null;
        this.name = null;
    }

    static registerPic(subclass) {
        this.knownPics.push(subclass);
    }

    static getPicByDevId(devID) {
        for (let picFamily of this.knownPics) {
            let picName = picFamily.deviceIdMap[devID];
            if (picName != null) {
                let pic = new picFamily();
                pic.devID = devID;
                pic.name = picName;
                pic.revID = 0;
                pic.userId = 0;
                return pic;
            }
        }
        return new GenericPIC();
    }


    /*** default methods ***/
    // if your PIC has different values than the ones defined here, override them

    getUserIdAddress() {
        return 0x8000;
    }

    getRevisionIdAddress() {
        return 0x8005;
    }
    
    getDeviceIdAddress() {
        return 0x8006;
    }

    getConfigWordsAddress() {
        return 0x8007;
    }

    getConfigWordsSize() {
        return 5;
    }

    getDiaAddress() {
        return 0x8100;
    }

    getDiaSize() {
        return 32;
    }

    getDciAddress() {
        return 0x8200;
    }

    getDciSize() {
        return 5;
    }

    getEEPROMAddress() {
        return 0xF000;
    }

    readDiaFields(diaFields) {
        console.log(`DIA Fields: ${diaFields}`);
        this.MUI = diaFields.slice(0, 9).map(value => value.toString(16).padStart(2, '0')).join('');
        this.OEUI = diaFields.slice(11, 18).map(value => value.toString(16).padStart(2, '0')).join('');
        this.TSLR1 = diaFields[19];
        this.TSLR2 = diaFields[20];
        this.TSHR1 = diaFields[21];
        this.TSHR2 = diaFields[22];        
    }

    readDciFields(dciFields) {
        console.log(`DCI Fields: ${dciFields}`);
        this.ERSIZ = dciFields[0]; // Erase Row Size
        this.WLSIZ = dciFields[1]; // Number of write latches per row
        this.URSIZ = dciFields[2]; // Number of user erasable rows
        this.EESIZ = dciFields[3]; // Data EEPROM memory size
        this.PCNT = dciFields[4]; // Pin Count
    }

    getEraseBits(flash=true, eeprom=false, userid=false, config=false) {
        let data = 0x00;
        if(eeprom) data |= 0x01;
        if(flash)  data |= 0x02;
        if(userid) data |= 0x04;
        if(config) data |= 0x08;
        return data;        
    }

    /**
     * Returns the LVP register address. 
     */
    getLVPConfigAddress() {
        return 0x800A;
    }

    /**
     * Returns the mask for the LVP configuration bits.
     * The mask is used to protect LVP bit to be set as 0 
     * Returns 0x2000 for the mask. 
     */
    getLVPSafeMask() {
        return 0x2000;
    }

    /*** virtual methods ***/
    // the methods defined here must be implemented by a child class of ICSP_HID
    
    getTpIntDelayMs() {
        throw new Error('Function getTpIntDelayMs is abstract and must be implemented');
    }

    getTpIntConfWordDelayMs() {
        throw new Error('Function getTpIntConfWordDelayMs is abstract and must be implemented');
    }

    getBulkEraseTimeMs() {
        throw new Error('Function getBulkEraseTimeMs is abstract and must be implemented');
    }

    getRowEraseTimeMs() {
        throw new Error('Function getRowEraseTimeMs is abstract and must be implemented');
    }
}

class PIC16F180XY extends GenericPIC {

    static deviceIdMap = {
        // PIC16F180XY
        0x30F1: "PIC16F18013",
        0x30F2: "PIC16F18014",
        0x30F5: "PIC16F18015",
        0x30F3: "PIC16F18023",
        0x30F4: "PIC16F18024",
        0x30F6: "PIC16F18025",
        0x30F9: "PIC16F18026",
        0x30F7: "PIC16F18044",
        0x30F8: "PIC16F18045",
        0x30FA: "PIC16F18046",
        0x30FB: "PIC16F18054",
        0x30FC: "PIC16F18055",
        0x30FF: "PIC16F18056",
        0x30FD: "PIC16F18074",
        0x30FE: "PIC16F18075",
        0x3100: "PIC16F18076",
    }

    constructor() {
        super();
    }

    getTpIntDelayMs() {
        return 2.5;
    }

    getTpIntConfWordDelayMs() {
        return 6;
    }

    getBulkEraseTimeMs() {
        return 13; 
    }

    getRowEraseTimeMs() {
        return 2.5; 
    }
}
GenericPIC.registerPic(PIC16F180XY);

class PIC16F131XY extends GenericPIC {

    static deviceIdMap = {
        // PIC16F131XY
        0x3121: "PIC16F13113",
        0x3124: "PIC16F13114",
        0x3127: "PIC16F13115",
        0x3122: "PIC16F13123",
        0x3125: "PIC16F13124",
        0x3128: "PIC16F13125",
        0x3123: "PIC16F13143",
        0x3126: "PIC16F13144",
        0x3129: "PIC16F13145",
    }

    constructor() {
        super();
    }

    getTpIntDelayMs() {
        return 7.5;
    }

    getTpIntConfWordDelayMs() {
        return 13;
    }

    getBulkEraseTimeMs() {
        return 20; 
    }

    getRowEraseTimeMs() {
        return 9.5; 
    }
}
GenericPIC.registerPic(PIC16F131XY);