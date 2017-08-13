const fetch = require('node-fetch');
const scrape = require('scrape-it');
const notifier = require('node-notifier');
const chalk = require('chalk');
const dotenv = require('dotenv');

dotenv.load();

let lastValue;
getTimeStamp = () => {
  const t = new Date();
  return t.toISOString().substring(11, t.length);
};

let usdToIsk;
const totalSignatumCoins = process.env.TOTALSIGT || -1;
if (totalSignatumCoins < 0) {
  console.log('Define TOTALSIGT=<value> in a .env file');
}

const getIskCurrency = () => {
  scrape('https://www.islandsbanki.is/', {
    usd: {
      selector: '.table-currency tbody tr[data-currency=usd] td.text-right',
      eq: 1,
    },
  }).then(data => {
    usdToIsk = parseInt(data.usd);
  });
};
getIskCurrency();

const getData = () => {
  fetch('https://www.cryptopia.co.nz/Exchange/GetListData?_=1502053453124')
    .then(r => r.json())
    .then(data => {
      return data.filter(item => item.Symbol === 'SIGT' && item.BaseSymbol === 'BTC');
    })
    .then(data => {
      if (data.length === 1) {
        const lastTrade = data[0].LastTrade;
        const sigValue = lastTrade * totalSignatumCoins;
        fetch('https://api.coinbase.com/v2/prices/spot\?currency\=USD')
          .then(r => r.json())
          .then(data => {
            const currentValue = Math.floor(sigValue * data.data.amount);
            process.stdout.clearLine();
            process.stdout.cursorTo(0);
            console.log(
              '[' + chalk.red(getTimeStamp()) + ']',
              currentValue + ' USD   -   ',
              chalk.green(
                (currentValue * usdToIsk).toString().replace(/\B(?=(\d{3})+(?!\d))/g, '.')
              ),
              ' ISK '
            );
            process.stdout.write(
              `1 Signatum value: ${lastTrade} , Total Bitcoin value: ${sigValue} `
            );
            if (!lastValue) {
              lastValue = currentValue;
            }
            const difference = currentValue > lastValue
              ? currentValue - lastValue
              : lastValue - currentValue;
            if (difference > 10) {
              notifier.notify({
                title: `${currentValue} USD - ${(currentValue * usdToIsk)
                  .toString()
                  .replace(/\B(?=(\d{3})+(?!\d))/g, '.')} ISK`,
                message: currentValue > lastValue
                  ? `✅ Price going up, last value was ${lastValue}`
                  : `😡 Price going down, last value was ${lastValue}`,
              });
              lastValue = currentValue;
            }
          });
      } else {
        console.log('Got ' + data.length + ' results');
      }
    });
};
const intervalMilliSec = 30000;
const currencyInterval = 2 * 3600000;
console.log(
  `Interval set to: ${chalk.magenta(intervalMilliSec / 1000)} seconds, scrape ${chalk.green('ISK')} currency every ${chalk.magenta(currencyInterval / 3600000)} hours`
);
setInterval(getData, intervalMilliSec);
setInterval(getIskCurrency, currencyInterval);
