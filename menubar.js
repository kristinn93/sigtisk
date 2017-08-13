const menubar = require('menubar');
const path = require('path');

const mb = menubar({
  tooltip: 'Wow such toolbar',
  index: path.join('file://', __dirname, 'index.html'),
});

mb.on('ready', function ready() {
  console.log('app is ready');
  title = 'Your app is ready';
  // your app code here
});
