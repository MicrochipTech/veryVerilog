let icsp_hid = null;
let hexObject = '';
let memory = null;

function loadLessons(){
    const lessonsContainer = document.getElementById('lessonsContainer');
    for (var i = 0; i < lessons; i++) {
        console.log('Iteration:', i);
        const button = document.createElement('button');
        button.classList.add('lesson-button');
        button.textContent = `Lesson${i}`;
        button.addEventListener('click', () => {
            const buttonText = button.textContent.toLowerCase();
            window.open(`lessons/${buttonText}/index2.html`, '_blank');
        });
        lessonsContainer.appendChild(button);
    }
}

function disconnectHID() {
    icsp_hid.disconnect();
    icsp_hid = null;
    $('#connect').text("Connect to Programmer");
    $('#connect').addClass("btn-primary");
    $('#connect').removeClass("btn-danger");
    $('#identify').prop('disabled', true);
    $('#settings').prop('disabled', true);
    $('#drop-area').hide();
    $('#picInfo').hide();
    memory = null;
}

function showInfoHID(params) {
    $("#picName").text(icsp_hid.getDeviceNameById(icsp_hid.devID));
    $("#userId").html("<strong>UserId:&nbsp;</strong>"+icsp_hid.userId);
    $('#picInfo').show();
}

function showPicDetails(){
    $('#modalTitle').text($('#picName').text());
    $('#modalBody').empty();
    $('#modalBody').html('<table class="table table-bordered" id="picInfoTable">'
        +'<thead><tr><th>Parameter</th><th>Value</th></tr></thead>'
        +'<tbody></tbody></table>'
    );
    fields = ["devIDx", "revIDx", "MUI", "ERSIZ", "WLSIZ", "URSIZ", "EESIZ", "PCNT"];
    fields.forEach(element => {
        $('#modalBody tbody').append(
            '<tr><td>' + element + '</td><td>' + icsp_hid[element] + '</td></tr>'
        );
    });
    $('#dataModal').modal('show');
}

async function showModalMessage(title="", msg="", text=true){
    $('#modalTitle').text(title);
    $('#modalBody').empty();
    if(text) $('#modalBody').text(msg);
    else $('#modalBody').html(msg);
    $('#dataModal').modal('show');
    return new Promise(resolve => 
        $('#closeModal').on('click', () => {
                $('#dataModal').modal('hide');
                resolve();
            }
        )
    );
}

function showNoPicDetected(){
    showModalMessage("Error", "No PIC detected or PIC is unknown.");
}

function showNoValidHexFile(){
    showModalMessage("Error", "Drag a valid Hex File on the drop area to program the PIC.");
}

function showPicProgrammed(){
    showModalMessage("Success", "PIC programmed successfully.");
}

async function programmDevice(){
    let args = [
        flash = $("#program-space").prop('checked'),
        eeprom = $("#eeprom-space").prop('checked'),
        userid = $("#userid-space").prop('checked'),
        config = $("#config-bits-space").prop('checked'),
    ];
    if(!await icsp_hid.eraseDevice(...args)){
        showModalMessage("Error", "Could not erase device");
        return false;
    }
    args.push["verify"] = $("#verify").prop('checked');
    if(!await icsp_hid.programEntireDevice(hexObject, ...args)){
        showModalMessage("Error", "Could not write to the flash");
        return false;
    }
    $("#userId").html("<strong>UserId:&nbsp;</strong>"+icsp_hid.userId);
    return true;
}

async function showMemory(){
    if(memory == null) {
        $('#modalTitle').text("Device Memory");
        $('#modalBody').empty();
        $('#modalBody').text("No data to show. Read the device first.");
        $('#dataModal').modal('show');
        return;
    }
    $('#modalTitle').text("Device Memory");
    $('#modalBody').empty();
    let fields = {
        "memory": ["Program Flash", 16], 
        "eeprom": ["EEPROM", 8],
        "userId": ["UserId", 4,], 
        "configWords": ["Config Words", 1]
    };
    $('#modalBody').html(
        '<div class="btn-group" id="memmoryBtn"></div><div id="memmoryTables"></div>'
    );
    for(const [key, value] of Object.entries(fields)) {
        // create entry for memory area
        $('#memmoryBtn').append(
            `<button type="button" class="btn btn-secondary" data-bs-toggle="collapse" `
            + `data-bs-target="#${key}Div" aria-expanded="false" aria-controls="#${key}Div">${value[0]}</button>`
        );
        $('#memmoryTables').append(
            `<div class="collapse" data-bs-parent="#memmoryTables" id="${key}Div">`
            + `<table id="${key}Table" class="table table-sm"></table></div>`);
            
        // create table header and body. header spans over the amount of columns defined on 'value'
        $('#' + key + 'Table').append(`<thead><tr><th>Address</th><th colspan="${value[1]}">Data</th></tr></thead>`);
        $('#' + key + 'Table').append(`<tbody></tbody>`);
        const tableBody = document.getElementById(key + 'Table').getElementsByTagName('tbody')[0];

        if(key in memory) {
            let offset = memory[key + "Address"];
            for (let i = 0; i < memory[key].length;) {
                const row = tableBody.insertRow();
                const cell1 = row.insertCell(0);
                cell1.textContent = `0x${(i + offset).toString(16).padStart(4, '0').toUpperCase()}`;
                for(let j = 0; j<value[1]; j++) {
                    const cell = row.insertCell();
                    cell.textContent = `${memory[key][i].toString(16).padStart(key==="eeprom"?2:4, '0').toUpperCase()}`;
                    i++;
                }
            }        
        }
        else {
            // replace colspan to 1 as we have only one column
            $('#' + key + 'Table thead th:last-child').attr('colspan', 1);
            const row = tableBody.insertRow();
            const cell1 = row.insertCell(0);
            const cell2 = row.insertCell(1);
            cell1.textContent = `-`;
            cell2.textContent = `-`;
        }
    }
    $('#dataModal').modal('show');
}

async function readDevice(){
    try {
        $('#modalTitle').text("Device Memory");
        $('#modalBody').empty();
        $('#modalBody').text("Reading...  Please wait.");
        $('#dataModal').modal('show');
        memory = await icsp_hid.readDevice();
        await showMemory();
    }
    catch(e) {
        console.error('There was an error reading the HID device:', e);
        showModalMessage("Error", "There was an error reading the HID device: " + e.message);
    }
}

async function identifyProgrammer() {
    showModalMessage("Status", "Leds on ICSP programmer are blinking...");
    await icsp_hid.blinkLeds();
    $('#dataModal').modal('hide');
}

async function connectProgrammer() {
    if($('#connect').hasClass("btn-primary")) {
        try {
            icsp_hid = new GenericPIC();
            if(await icsp_hid.connect()) {
                await icsp_hid.getConnectionInfo();
                showInfoHID();
                $('#connect').text("Disconnect Programmer");
                $('#connect').removeClass("btn-primary");
                $('#connect').addClass("btn-danger");
                $('#identify').prop('disabled', false);
                $('#settings').prop('disabled', false);
                $('#drop-area').show();
            }
        } catch (e) {
            console.error('There was an error communicating with the HID device:', e);
            showModalMessage("Error", "There was an error communicating with the HID device: " + e.message);
            disconnectHID();
        }
    } else {
        disconnectHID();
    }
}

function handleDroppedFile(event) {
    event.preventDefault();
    event.stopPropagation();

    $(this).removeClass('border-primary');
    const dataTransfer = event.originalEvent.dataTransfer;
    if (dataTransfer && dataTransfer.files.length) {
        const file = dataTransfer.files[0];
        const reader = new FileReader();
        reader.onload = function(e) {
            let intelHexString = e.target.result;
            hexObject = MemoryMap.fromHex(intelHexString);
            if($("#drag-and-flash").prop("checked")) {
                $('#programit').click();
            }
        };
        reader.readAsText(file);
    }
}

async function triggerProgrammer() {
    if($("#picName").text().indexOf("PIC") !== -1) {
        if (hexObject){
            if(await programmDevice()) {
                showPicProgrammed();
            }
        }
        else {
            showNoValidHexFile();
        } 
    }             
    else {
        showNoPicDetected();
    }
}

async function readProgrammer() {
    if($("#picName").text().indexOf("PIC") !== -1) {
        await readDevice();
    }
    else {
        showNoPicDetected();
    }
}

async function showProgrammerMemory() {
    if($("#picName").text().indexOf("PIC") !== -1) {
        await showMemory();
    }
    else {
        showNoPicDetected();
    }
}

// Check if the Web Serial API is supported
if ("serial" in navigator) {

    // comment this line for debugging
    console.log = function() {}

    // The Web Serial API is supported.
    $(document).ready(function() {

        const buttonContainer = document.getElementById('lessonsContainer');
        buttonContainer.innerHTML = ''; 
        loadLessons();
        $('#picInfo').hide();
        $('#drop-area').hide();

        $('#connect').click(connectProgrammer);

        $('#drop-area').on('dragover', function(event) {
            event.preventDefault();
            event.stopPropagation();
            $(this).addClass('border-primary');
        });
        $('#drop-area').on('dragleave', function(event) {
            event.preventDefault();
            event.stopPropagation();
            $(this).removeClass('border-primary');
        });
        $('#drop-area').on('drop', handleDroppedFile);

        $('#picInfo').click(showPicDetails);
        $('#closeModal').click(function(){ 
            $('#dataModal').modal('toggle');
        });
        $('#identify').click(identifyProgrammer);
        $('#programit').click(triggerProgrammer);
        $('#readit').click(readProgrammer);
        $('#showit').click(showProgrammerMemory);
    });
} else {
    alert("Web Serial API not supported.");
}

// Handle nested dropdowns
var dropdownSubmenus = document.querySelectorAll('.dropdown-submenu');
dropdownSubmenus.forEach(function (submenu) {
  submenu.addEventListener('click', function (e) {
    e.stopPropagation();
    var subMenu = submenu.querySelector('.dropdown-menu');
    subMenu.classList.toggle('show');
  });
});

// Prevent closing the dropdown when clicking inside the submenu
var dropdownItems = document.querySelectorAll('.dropdown-submenu .dropdown-menu li');
dropdownItems.forEach(function (item) {
  item.addEventListener('click', function (e) {
    e.stopPropagation();
  });
});

 // Hide submenus when the main dropdown collapses
 var mainDropdown = document.querySelector('.btn-group');
 mainDropdown.addEventListener('hidden.bs.dropdown', function () {
   var subMenus = document.querySelectorAll('.dropdown-submenu .dropdown-menu');
   subMenus.forEach(function (subMenu) {
     subMenu.classList.remove('show');
   });
 });
