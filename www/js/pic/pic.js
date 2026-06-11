
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
        let pic = new GenericPIC();
        pic.devID = devID;
        pic.name = `Unknown PIC with DEVID 0x${devID.toString(16).toUpperCase().padStart(4, '0')}`;
        pic.revID = 0;
        pic.userId = 0;
        return pic;
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

    /**
     * Returns true if this PIC uses dedicated Read Device ID / Revision ID commands
     * instead of setPC + readWord. Override in families that have these commands.
     */
    hasDirectDeviceIdCmd() {
        return false;
    }

    /**
     * Returns the opcode for the Read Device ID command (used when hasDirectDeviceIdCmd() is true).
     */
    getReadDeviceIdCmd() {
        return 0x24;
    }

    /**
     * Returns the opcode for the Read Revision ID command (used when hasDirectDeviceIdCmd() is true).
     */
    getReadRevisionIdCmd() {
        return 0x28;
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
        return 13.5;
    }

    getBulkEraseTimeMs() {
        return 20; 
    }

    getRowEraseTimeMs() {
        return 9.5; 
    }
}
GenericPIC.registerPic(PIC16F131XY);

class PIC16F132XY extends GenericPIC {

    static deviceIdMap = {
        // PIC16F132XY
        0x3130: "PIC16F13214",
        0x3135: "PIC16F13215",
        0x3131: "PIC16F13224",
        0x3136: "PIC16F13225",
        0x3141: "PIC16F13226",
        0x3132: "PIC16F13244",
        0x3137: "PIC16F13245",
        0x3142: "PIC16F13246",
        0x3133: "PIC16F13254",
        0x3138: "PIC16F13255",
        0x3143: "PIC16F13256",
        0x3134: "PIC16F13274",
        0x3139: "PIC16F13275",
        0x3144: "PIC16F13276",
    }

    constructor() {
        super();
    }

    getTpIntDelayMs() { // TPINT DS40002646A pg.32
        return 8;
    }

    getTpIntConfWordDelayMs() { // TPINT DS40002646A pg.32
        return 13.5;
    }

    getBulkEraseTimeMs() { // TERAB DS40002646A pg.31
        return 40; 
    }

    getRowEraseTimeMs() { // TERAR DS40002646A pg.32
        return 10; 
    }
}
GenericPIC.registerPic(PIC16F132XY);

class PIC18FQ35 extends GenericPIC {

    static deviceIdMap = {
        0x7C20: "PIC18F24Q35",
        0x7C40: "PIC18F25Q35",
        0x7C60: "PIC18F26Q35",
        0x7C80: "PIC18F44Q35",
        0x7CA0: "PIC18F45Q35",
        0x7CC0: "PIC18F46Q35",
        0x7CE0: "PIC18F54Q35",
        0x7D00: "PIC18F55Q35",
        0x7D20: "PIC18F56Q35",
    }

    constructor() {
        super();
    }

    // PIC18F-Q35 DS40002659A §2.2: Device ID at 0x3FFFFEh, Revision ID at 0x3FFFFCh
    getDeviceIdAddress()   { return 0x3FFFFE; }
    getRevisionIdAddress() { return 0x3FFFFC; }

    // DS40002659A §2.1: User IDs — 32 words at 0x200000–0x20003F
    getUserIdAddress()     { return 0x200000; }

    // DS40002659A §2.5: Configuration Bytes at 0x300000–0x300014 (13 bytes)
    getConfigWordsAddress() { return 0x300000; }
    getConfigWordsSize()    { return 13; }

    // DS40002659A §2.3: DIA at 0x2C0000–0x2C00FFh
    getDiaAddress() { return 0x2C0000; }
    getDiaSize()    { return 32; }

    // DS40002659A §2.4: DCI at 0x3C0000–0x3C0009h
    getDciAddress() { return 0x3C0000; }
    getDciSize()    { return 5; }

    // DS40002659A §2 Table 2-1: Data EEPROM at 0x380000–0x3800FFh
    getEEPROMAddress() { return 0x380000; }

    // LVP config bit is at CONFIG4 (offset 0x300003), bit 5 (LVP)
    getLVPConfigAddress() { return 0x300003; }
    getLVPSafeMask()      { return 0x20; }

    // PIC18F-Q35 has dedicated Read Device ID (0x24) and Read Revision ID (0x28) commands
    // DS40002659A Table 3-1
    hasDirectDeviceIdCmd()  { return true; }
    getReadDeviceIdCmd()    { return 0x24; }
    getReadRevisionIdCmd()  { return 0x28; }

    // DS40002659A §4 Table 4-1 electrical specs
    // T_PINT = 75 µs (PFM and User IDs)
    // T_PDFM = 11 ms (EEPROM and Config bytes)
    // T_ERAB = 11 ms (bulk erase)
    // T_ERAS = 11 ms (page erase)
    getTpIntDelayMs()         { return 0.075; }  // 75 µs
    getTpIntConfWordDelayMs() { return 11; }      // 11 ms
    getBulkEraseTimeMs()      { return 11; }      // 11 ms
    getRowEraseTimeMs()       { return 11; }      // 11 ms

    readDiaFields(diaFields) {
        // DIA layout (DS40002659A Table 2-2): same offsets as PIC16F families
        // MUI0-8: words 0-8 (9 words), MUI9 reserved (1 word), EUI0-7: words 10-17 (8 words)
        // TSLR/TSHR/FVRA/FVRC follow
        this.MUI = diaFields.slice(0, 9).map(v => v.toString(16).padStart(4, '0')).join('');
        this.OEUI = diaFields.slice(10, 18).map(v => v.toString(16).padStart(4, '0')).join('');
    }

    readDciFields(dciFields) {
        // DCI layout (DS40002659A Table 2-3): addresses 0x3C0000–0x3C0008 (5 words)
        this.ERSIZ = dciFields[0]; // 128 words
        this.WLSIZ = dciFields[1]; // 0 (one-word-at-a-time write)
        this.URSIZ = dciFields[2]; // 64/128/256 pages depending on variant
        this.EESIZ = dciFields[3]; // 256 bytes
        this.PCNT  = dciFields[4]; // 28/40/48 pins
    }
}
GenericPIC.registerPic(PIC18FQ35);
