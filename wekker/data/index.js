var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event)
{
    initWebSocket();
    initButtons();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket()
{
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event)
{
    console.log('Connection opened');
}

function onClose(event)
{
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event)
{
    let data = JSON.parse(event.data);
    document.getElementById('led').className = data.status;
    document.getElementById('alarm_state').innerHTML = data.status;
    document.getElementById('wakeuptime').innerHTML = data.wakeuptime;
    document.getElementById('uptime').innerHTML = data.uptime;
    document.getElementById('reboot_count').innerHTML = data.reboot_count;
    document.getElementById("time").value = data.wakeuptime;
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

function initButtons()
{
    document.getElementById('submit').addEventListener('click', onSubmit);
    document.getElementById('toggle').addEventListener('click', onToggle);
}

function onToggle(event)
{
    websocket.send(JSON.stringify({'action':'toggle'}));
}

function onSubmit(event)
{
    const timeValue = document.getElementById("time").value;
    //var wakeuptime = document.getElementById('time').innerHTML;
    websocket.send(JSON.stringify({'submit':timeValue}));
}