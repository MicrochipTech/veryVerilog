let icsp_hid = null;
let hexObject = '';
let memory = null;

// Modal Manager - Centralized modal handling
const ModalManager = {
    _currentTimer: null,
    _onCloseCallback: null,

    show(title, content, options = {}) {
        // Clear any existing timer or callback
        this._cleanup();

        // Set modal content
        $('#modalTitle').text(title);
        $('#modalBody').empty();

        if (typeof content === 'string') {
            $('#modalBody').html(content);
        } else {
            $('#modalBody').append(content);
        }

        // Handle close button visibility
        if (options.hideCloseButton) {
            $('#closeModal').hide();
        } else {
            $('#closeModal').show();
        }

        // Setup close callback if provided
        if (options.onClose) {
            this._onCloseCallback = options.onClose;
            $('#closeModal').off('click').on('click', () => {
                this.hide();
                if (this._onCloseCallback) {
                    this._onCloseCallback();
                    this._onCloseCallback = null;
                }
            });
        } else {
            $('#closeModal').off('click').on('click', () => this.hide());
        }

        // Auto-close timer
        if (options.autoCloseMs) {
            this._currentTimer = setTimeout(() => {
                this.hide();
                if (this._onCloseCallback) {
                    this._onCloseCallback();
                    this._onCloseCallback = null;
                }
            }, options.autoCloseMs);
        }

        $('#dataModal').modal('show');
    },

    hide() {
        this._cleanup();
        $('#dataModal').modal('hide');
    },

    _cleanup() {
        if (this._currentTimer) {
            clearTimeout(this._currentTimer);
            this._currentTimer = null;
        }
        $('#closeModal').off('click');
    },

    // Specific modal types
    showMessage(title, message, isHtml = false) {
        const content = isHtml ? message : $('<div>').text(message).html();
        this.show(title, content);
    },

    showError(message) {
        this.showMessage('Error', message);
    },

    showSuccess(message, autoCloseSeconds = null) {
        if (autoCloseSeconds) {
            const autoCloseMs = autoCloseSeconds * 1000;
            const updateInterval = 50;
            const totalSteps = autoCloseMs / updateInterval;
            let currentStep = 0;

            const progressHtml = `
                <div class="mb-3">${message}</div>
                <div class="progress" style="height: 20px;">
                    <div id="autoCloseProgress" class="progress-bar progress-bar-striped progress-bar-animated bg-success"
                         role="progressbar" style="width: 100%;" aria-valuenow="100" aria-valuemin="0" aria-valuemax="100">
                        Closing in <span id="countdownText">${autoCloseSeconds}</span>s
                    </div>
                </div>
            `;

            this.show('Success', progressHtml, { autoCloseMs });

            // Update countdown
            const intervalId = setInterval(() => {
                currentStep++;
                const remainingPercent = 100 - (currentStep / totalSteps * 100);
                const remainingSeconds = Math.ceil((autoCloseSeconds * 1000 - currentStep * updateInterval) / 1000);

                $('#autoCloseProgress').css('width', remainingPercent + '%');
                $('#autoCloseProgress').attr('aria-valuenow', remainingPercent);
                $('#countdownText').text(remainingSeconds);

                if (currentStep >= totalSteps) {
                    clearInterval(intervalId);
                }
            }, updateInterval);

            // Store interval ID for cleanup
            this._currentTimer = intervalId;
        } else {
            this.showMessage('Success', message);
        }
    },

    showProgress(title, initialMessage) {
        const progressHtml = `
            <div id="progressMessage" class="mb-2">${initialMessage}</div>
            <div class="progress" style="height: 8px;">
                <div id="progressBar" class="progress-bar progress-bar-striped progress-bar-animated bg-primary"
                     role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">
                </div>
            </div>
        `;
        this.show(title, progressHtml, { hideCloseButton: true });
    },

    updateProgress(percent, message) {
        $('#progressBar').css('width', percent + '%');
        $('#progressBar').attr('aria-valuenow', percent);
        if (message) {
            $('#progressMessage').text(message);
        }
    }
};

function loadLessons(){
    const $lessonsContainer = $('#lessonsContainer');
    for (var i = 0; i < lessons; i++) {
        console.log('Iteration:', i);
        const $button = $('<button>')
            .addClass('lesson-button')
            .text(`Lesson${i}`)
            .on('click', function() {
                const buttonText = $(this).text().toLowerCase();
                window.open(`lessons/${buttonText}/index2.html`, '_blank');
            });
        $lessonsContainer.append($button);
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
    const tableHtml = '<table class="table table-bordered" id="picInfoTable">'
        +'<thead><tr><th>Parameter</th><th>Value</th></tr></thead>'
        +'<tbody></tbody></table>';

    ModalManager.show($('#picName').text(), tableHtml);

    const fields = ["devIDx", "revIDx", "MUI", "ERSIZ", "WLSIZ", "URSIZ", "EESIZ", "PCNT"];
    fields.forEach(element => {
        $('#picInfoTable tbody').append(
            '<tr><td>' + element + '</td><td>' + icsp_hid.pic[element] + '</td></tr>'
        );
    });
    $('#programmerInfoTable tbody').append(
            '<tr><td>HW UART baud</td><td id="HWUART_BAUD_VALUE">Reading programmer...</td></tr>'
    );
    icsp_hid.getHwBaudRate()
    .then(baud => {
        $('#HWUART_BAUD_VALUE').text(baud ? baud : "115200 (*)");
    })
    .catch(err => {
        $('#HWUART_BAUD_VALUE').text("115200 (*)");
        console.error('Get Baudrate error: HIDD comm issue ('+err+')');
    })
}

// Legacy function wrappers for compatibility
function showNoPicDetected(){
    ModalManager.showError("No PIC detected or PIC is unknown.");
}

function showNoValidHexFile(){
    ModalManager.showError("Drag a valid Hex File on the drop area to program the PIC.");
}

function showPicProgrammed(){
    ModalManager.showSuccess("PIC programmed successfully.", 5);
}

async function programmDevice(){
    let args = [
        flash = $("#program-space").prop('checked'),
        eeprom = $("#eeprom-space").prop('checked'),
        userid = $("#userid-space").prop('checked'),
        config = $("#config-bits-space").prop('checked'),
    ];

    // Show progress modal
    ModalManager.showProgress("Programming Device", "Erasing device...");

    if(!await icsp_hid.eraseDevice(...args)){
        ModalManager.hide();
        ModalManager.showError("Could not erase device");
        return false;
    }

    // set/clear the verify flag
    // when writing to the device's memory, it will be checked if the contents are
    // the same as expected. This will cause a slower programming time.
    icsp_hid.setVerify($("#verify").prop('checked'));

    // Set progress callback
    icsp_hid.setProgressCallback((progress, status) => {
        ModalManager.updateProgress(progress * 100, status);
    });

    try {
        await icsp_hid.programEntireDevice(hexObject, ...args);
    } catch(e) {
        icsp_hid.setProgressCallback(null);
        ModalManager.hide();
        ModalManager.showError(String(e));
        return false;
    }

    icsp_hid.setProgressCallback(null);
    ModalManager.hide();
    $("#userId").html("<strong>UserId:&nbsp;</strong>"+icsp_hid.pic.userId);
    return true;
}

async function showMemory() {
    if(memory == null){
        ModalManager.showMessage("Device Memory", "No data to show. Read the device first.");
        return;
    }

    const fields = {
        "memory": ["Program Flash", 16],
        "eeprom": ["EEPROM", 8],
        "userId": ["UserId", 4],
        "configWords": ["Config Words", 1]
    };

    const contentHtml = '<div class="btn-group" id="memmoryBtn"></div><div id="memmoryTables"></div>';
    ModalManager.show("Device Memory", contentHtml);

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
        const $tableBody = $('#' + key + 'Table tbody');

        if(key in memory) {
            let offset = memory[key + "Address"];
            for (let i = 0; i < memory[key].length;) {
                const $row = $('<tr>');
                const $cell1 = $('<td>').text(`0x${(i + offset).toString(16).padStart(4, '0').toUpperCase()}`);
                $row.append($cell1);
                for (let j = 0; j < value[1]; j++) {
                    const $cell = $('<td>').text(`${memory[key][i].toString(16).padStart(key === "eeprom" ? 2 : 4, '0').toUpperCase()}`);
                    $row.append($cell);
                    i++;
                }
                $tableBody.append($row);
            }
        } else {
            // replace colspan to 1 as we have only one column
            $('#' + key + 'Table thead th:last-child').attr('colspan', 1);
            const $row = $('<tr>');
            const $cell1 = $('<td>').text('-');
            const $cell2 = $('<td>').text('-');
            $row.append($cell1).append($cell2);
            $tableBody.append($row);
        }
    }
}

async function readDevice(){
    try {
        // Show progress modal
        ModalManager.showProgress("Reading Device", "Starting read...");

        // Set progress callback
        icsp_hid.setProgressCallback((progress, status) => {
            ModalManager.updateProgress(progress * 100, status);
        });

        memory = await icsp_hid.readDevice();

        icsp_hid.setProgressCallback(null);
        ModalManager.hide();

        await showMemory();
    }
    catch(e) {
        icsp_hid.setProgressCallback(null);
        ModalManager.hide();
        console.error('There was an error reading the HID device:', e);
        ModalManager.showError("There was an error reading the HID device: " + e.message);
    }
}

async function identifyProgrammer() {
    ModalManager.showMessage("Status", "Leds on ICSP programmer are blinking...");
    await icsp_hid.blinkLeds();
    ModalManager.hide();
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
        ModalManager.showError("Failed to reset target device: " + e.message);
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
            ModalManager.showError("There was an error communicating with the HID device: " + e.message);
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
    $('#versionInfo').text(`v${WEB_ICSP_VERSION}`);
    $(document).attr('title', `WebICSP v${WEB_ICSP_VERSION}`);
}

// Check if the Web Serial API is supported
if ("serial" in navigator) {

    // comment this line for debugging
    console.log = function() {}

    // The Web Serial API is supported.
    $(document).ready(function() {

        $('#lessonsContainer').empty();
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
        $('[data-bs-toggle="tooltip"]').each(function() {
            new bootstrap.Tooltip(this);
        });

        //handle programmer disconnected from usb
        navigator.hid.addEventListener('disconnect', (event) => {
            if (event.device === icsp_hid.hid) { disconnectHID(); }
        });
    });
} else {
    alert("Web Serial API not supported.");
}

// Handle nested dropdowns
$('.dropdown-submenu').on('click', function(e) {
    e.stopPropagation();
    $(this).find('.dropdown-menu').toggleClass('show');
});

// Prevent closing the dropdown when clicking inside the submenu
$('.dropdown-submenu .dropdown-menu li').on('click', function(e) {
    e.stopPropagation();
});

// Hide submenus when the main dropdown collapses
$('.btn-group').on('hidden.bs.dropdown', function() {
    $('.dropdown-submenu .dropdown-menu').removeClass('show');
});
