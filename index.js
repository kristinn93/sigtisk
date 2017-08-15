const menubar = require('menubar');
const path = require('path');
const BrowserWindow = require('electron').BrowserWindow;
const fetch = require('node-fetch');
const WebSocket = require('ws');
var fs = require('fs');

const upArrow = './up.png';
const downArrow = './down.png';

const mb = menubar({
  tooltip: 'Wow such toolbar',
  index: 'http://localhost:3000', //path.join('file://', __dirname, 'index.html'),
});

function createWindow() {
  // Create the browser window.
  const mainWindow = new BrowserWindow({ width: 800, height: 600 });

  // and load the index.html of the app.
  mainWindow.loadURL('http://localhost:3000');

  // Open the DevTools.
  mainWindow.webContents.openDevTools();

  // Emitted when the window is closed.
  mainWindow.on('closed', function() {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
  });
}

mb.on('ready', function ready() {
  createWindow();
  mb.tray.setTitle('Starting up');
  setTitleBar();
  setInterval(setTitleBar, 3000);
});

let lastValue = 0;
const setTitleBar = () => {
  fetch('https://www.cryptopia.co.nz/Exchange/GetListData?_=1502053453124')
    .then(r => r.json())
    .then(data => {
      return data.filter(item => item.Symbol === 'SIGT' && item.BaseSymbol === 'BTC');
    })
    .then(data => {
      if (data.length === 1) {
        const lastTrade = data[0].LastTrade;
        if (lastTrade < lastValue) {
          mb.tray.setImage(downArrow);
        }
        if (lastTrade > lastValue) {
          mb.tray.setImage(upArrow);
        }
        lastValue = lastTrade;
        mb.tray.setTitle(lastTrade.toString());
      }
    });
};
