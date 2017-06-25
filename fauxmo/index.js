const FauxMo = require('fauxmojs');
const ip = require('ip');
const { spawnSync } = require('child_process');
const request = require('request');

let fauxMo = new FauxMo({
  ipAddress: ip.address(),
  devices: [{
    name: 'Light',
    port: 11000,
    handler(action) {
      if (action === 'on') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5272835'], { encoding: 'utf8'});
      } else if (action === 'off') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5272844'], { encoding: 'utf8'});
      }
    }
  }, {
    name: 'Christmas Tree',
    port: 11001,
    handler(action) {
      if (action === 'on') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5264691'], { encoding: 'utf8'});
      } else if (action === 'off') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5264700'], { encoding: 'utf8'});
      }
    }
  }, {
    name: 'Bedroom Light',
    port: 11002,
    handler(action) {
      if (action === 'on') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5264835'], { encoding: 'utf8'});
      } else if (action === 'off') {
        spawnSync('/home/pi/workspace/rf_pi/send', ['5264844'], { encoding: 'utf8'});
      }
    }
  }, {
    name: 'A/C',
    port: 11003,
    handler(action) {
      request.get('http://ac.local/ac_power');
    }
  }]
});

console.log('started...');