const menubar = require('menubar');
const path = require('path');
const BrowserWindow = require('electron').BrowserWindow;

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

mb.on('ready', createWindow);
