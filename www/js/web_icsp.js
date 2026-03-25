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
    $('#reset-target').prop('disabled', true);
    $('#drop-area').hide();
    $('#picInfo').hide();
    memory = null;
}

function showInfoHID(params) {
    $("#picName").text(icsp_hid.pic.name);
    $("#userId").html("<strong>UserId:&nbsp;</strong>"+icsp_hid.pic.userId);
    $('#picInfo').show();
}

function showPicDetails(){
    $('#modalTitle').text($('#picName').text());
    $('#modalBody').empty();
    $('#modalBody').html('<table class="table table-bordered" id="picInfoTable">'
        +'<thead><tr><th>MCU Parameter</th><th>Value</th></tr></thead>'
        +'<tbody></tbody></table>'
        +'<table class="table table-bordered" id="programmerInfoTable">'
        +'<thead><tr><th>Programmer parameter</th><th>Value</th></tr></thead>'
        +'<tbody></tbody></table>'
    );
    fields = ["devIDx", "revIDx", "MUI", "ERSIZ", "WLSIZ", "URSIZ", "EESIZ", "PCNT"];
    fields.forEach(element => {
        $('#modalBody #picInfoTable tbody').append(
            '<tr><td>' + element + '</td><td>' + icsp_hid.pic[element] + '</td></tr>'
        );
    });
    $('#modalBody #programmerInfoTable tbody').append(
            '<tr><td>HW UART baud</td><td id="HWUART_BAUD_VALUE">Reading programmer...</td></tr>'
        );
    $('#dataModal').modal('show');
    icsp_hid.getHwBaudRate()
    .then(baud => {
        $('#HWUART_BAUD_VALUE').text(baud ? baud : "115200 (*)");
    })
    .catch(err => {
        $('#HWUART_BAUD_VALUE').text("115200 (*)");
        console.error('Get Baudrate error: HIDD comm issue ('+err+')');
    })
}

function showProgressModal(title, message) {
    const progressHtml = `
        <div id="progressMessage" class="mb-2">${message}</div>
        <div class="progress" style="height: 8px;">
            <div id="progressBar" class="progress-bar progress-bar-striped progress-bar-animated bg-primary"
                 role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">
            </div>
        </div>
    `;
    $('#modalTitle').text(title);
    $('#modalBody').empty();
    $('#modalBody').html(progressHtml);
    $('#dataModal').modal('show');
}

function updateProgress(percent, message) {
    $('#progressBar').css('width', percent + '%');
    $('#progressBar').attr('aria-valuenow', percent);
    $('#progressMessage').text(message);
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

async function showPicProgrammed(){
    const autoCloseSeconds = 5;
    const updateInterval = 50; // Update every 50ms for smooth animation
    const totalSteps = (autoCloseSeconds * 1000) / updateInterval;
    let currentStep = 0;

    // Create modal content with progress bar
    const modalContent = `
        <div class="mb-3">PIC programmed successfully.</div>
        <div class="progress" style="height: 20px;">
            <div id="autoCloseProgress" class="progress-bar progress-bar-striped progress-bar-animated bg-success"
                 role="progressbar" style="width: 100%;" aria-valuenow="100" aria-valuemin="0" aria-valuemax="100">
                Closing in <span id="countdownText">${autoCloseSeconds}</span>s
            </div>
        </div>
    `;

    $('#modalTitle').text("Success");
    $('#modalBody').empty();
    $('#modalBody').html(modalContent);
    $('#dataModal').modal('show');

    // Setup interval for countdown and progress bar
    const intervalId = setInterval(() => {
        currentStep++;
        const remainingPercent = 100 - (currentStep / totalSteps * 100);
        const remainingSeconds = Math.ceil((totalSteps - currentStep) * updateInterval / 1000);

        $('#autoCloseProgress').css('width', remainingPercent + '%');
        $('#countdownText').text(remainingSeconds);

        if (currentStep >= totalSteps) {
            clearInterval(intervalId);
            $('#dataModal').modal('hide');
        }
    }, updateInterval);

    // Allow manual close and cleanup interval
    const closeHandler = () => {
        clearInterval(intervalId);
        $('#dataModal').modal('hide');
        $('#closeModal').off('click', closeHandler);
    };
    $('#closeModal').on('click', closeHandler);

    // Cleanup if modal is hidden by other means (ESC key, backdrop click)
    $('#dataModal').one('hidden.bs.modal', () => {
        clearInterval(intervalId);
        $('#closeModal').off('click', closeHandler);
    });
}

async function programmDevice(){
    let args = [
        flash = $("#program-space").prop('checked'),
        eeprom = $("#eeprom-space").prop('checked'),
        userid = $("#userid-space").prop('checked'),
        config = $("#config-bits-space").prop('checked'),
    ];

    // Show progress modal
    showProgressModal("Programming Device", "Erasing device...");

    if(!await icsp_hid.eraseDevice(...args)){
        $('#dataModal').modal('hide');
        showModalMessage("Error", "Could not erase device");
        return false;
    }

    // set/clear the verify flag
    // when writing to the device's memory, it will be checked if the contents are
    // the same as expected. This will cause a slower programming time.
    icsp_hid.setVerify($("#verify").prop('checked'));

    // Set progress callback
    icsp_hid.setProgressCallback((progress, status) => {
        updateProgress(progress * 100, status);
    });

    try {
        await icsp_hid.programEntireDevice(hexObject, ...args);
    } catch(e) {
        icsp_hid.setProgressCallback(null);
        $('#dataModal').modal('hide');
        showModalMessage("Error", e);
        return false;
    }

    icsp_hid.setProgressCallback(null);
    $('#dataModal').modal('hide');
    $("#userId").html("<strong>UserId:&nbsp;</strong>"+icsp_hid.pic.userId);
    return true;
}

async function showMemory() {
    if(memory == null){
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
        "userId": ["UserId", 4],
        "configWords": ["Config Words", 1]
    };
    $('#modalBody').html(
        '<div class="btn-group" id="memmoryBtn"></div><div id="memmoryTables"></div>'
    );
    for(const [key, value] of Object.entries(fields)) {
        // create entry for memory area
        $('#memmoryBtn').append(
            `<button type="button" class="btn btn-secondary" data-bs-toggle="collapse" `
            + `data-bs-target="#${key}Div" aria-expanded="false" aria-controls="${key}Div">${value[0]}</button>`
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
                for (let j = 0; j < value[1]; j++) {
                    const cell = row.insertCell();
                    cell.textContent = `${memory[key][i].toString(16).padStart(key === "eeprom" ? 2 : 4, '0').toUpperCase()}`;
                    i++;
                }
            }
        } else {
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
        // Show progress modal
        showProgressModal("Reading Device", "Starting read...");

        // Set progress callback
        icsp_hid.setProgressCallback((progress, status) => {
            updateProgress(progress * 100, status);
        });

        memory = await icsp_hid.readDevice();

        icsp_hid.setProgressCallback(null);
        $('#dataModal').modal('hide');

        await showMemory();
    }
    catch(e) {
        icsp_hid.setProgressCallback(null);
        $('#dataModal').modal('hide');
        console.error('There was an error reading the HID device:', e);
        showModalMessage("Error", "There was an error reading the HID device: " + e.message);
    }
}

async function identifyProgrammer() {
    showModalMessage("Status", "Leds on ICSP programmer are blinking...");
    await icsp_hid.blinkLeds();
    $('#dataModal').modal('hide');
}

async function resetTarget() {
    try {
        console.log('Resetting target: lvpEnter');
        await icsp_hid.lvpEnter();
        console.log('Waiting 100ms');
        await new Promise(resolve => setTimeout(resolve, 100));
        console.log('Resetting target: lvpExit');
        await icsp_hid.lvpExit();
    } catch (e) {
        console.error('Error resetting target device:', e);
        showModalMessage("Error", "Failed to reset target device: " + e.message);
    }
}

async function connectProgrammer() {
    if($('#connect').hasClass("btn-primary")) {
        try {
            icsp_hid = new ICSP_HID();
            if(await icsp_hid.connect()) {
                await icsp_hid.getConnectionInfo();
                showInfoHID();
                $('#connect').text("Disconnect Programmer");
                $('#connect').removeClass("btn-primary");
                $('#connect').addClass("btn-danger");
                $('#identify').prop('disabled', false);
                $('#settings').prop('disabled', false);
                $('#reset-target').prop('disabled', false);
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

function loadHexFile(file) {
    if (file) {
        const reader = new FileReader();
        reader.onload = function(e) {
            let intelHexString = e.target.result;
            hexObject = MemoryMap.fromHex(intelHexString);
            // Update drop area to show loaded file name
            $('#drop-area').html(`
                <div>HEX file loaded: <strong>${file.name}</strong></div>
                <div class="mt-3">
                    <button id="browse-file" class="btn btn-secondary">Browse for HEX file</button>
                    <input type="file" id="file-input" accept=".hex" style="display: none;">
                </div>
            `);
            // Re-attach event listeners after updating HTML
            $('#browse-file').click(function() {
                $('#file-input').click();
            });
            $('#file-input').change(handleFileSelect);

            if($("#drag-and-flash").prop("checked")) {
                $('#programit').click();
            }
        };
        reader.readAsText(file);
    }
}

function handleDroppedFile(event) {
    event.preventDefault();
    event.stopPropagation();

    $(this).removeClass('border-primary');
    const dataTransfer = event.originalEvent.dataTransfer;
    if (dataTransfer && dataTransfer.files.length) {
        loadHexFile(dataTransfer.files[0]);
    }
}

function handleFileSelect(event) {
    const file = event.target.files[0];
    if (file) {
        loadHexFile(file);
    }
    // Reset the input value so the same file can be selected again
    event.target.value = '';
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

// Display the version of the WEB ICSP Programmer
function displayVersion() {
    const versionElement = document.getElementById('versionInfo');
    versionElement.textContent = `v${WEB_ICSP_VERSION}`;
    document.title = `WebICSP v${WEB_ICSP_VERSION}`;
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

        // Browse button and file input handlers
        $(document).on('click', '#browse-file', function() {
            $('#file-input').click();
        });
        $(document).on('change', '#file-input', handleFileSelect);

        $('#picInfo').click(showPicDetails);
        $('#closeModal').click(function(){
            $('#dataModal').modal('toggle');
        });
        $('#identify').click(identifyProgrammer);
        $('#reset-target').click(resetTarget);
        $('#programit').click(triggerProgrammer);
        $('#readit').click(readProgrammer);
        $('#showit').click(showProgrammerMemory);

        displayVersion(); // Call the function to display the version

        //Initialize tooltips 
        const tooltipTriggerList = document.querySelectorAll('[data-bs-toggle="tooltip"]')
        const tooltipList = [...tooltipTriggerList].map(tooltipTriggerEl => new bootstrap.Tooltip(tooltipTriggerEl))

        //handle programmer disconnected from usb
        navigator.hid.addEventListener('disconnect', (event) => {
            if (event.device === icsp_hid.hid) { disconnectHID(); }
        });
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
