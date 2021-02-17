class Riac {

	constructor(){
		this.evb = new EveBrain('ws://192.168.4.1:8899/websocket');
	}

	analog0() {
		logo.hold = true;
		this.evb.analogInput(0, next);
		function next(val) {
			logo.hold = false; 
			logo.arglist.pop(); 
			logo.arglist.push(val);
		};
	}

	getIp() {
		logo.hold = true;
		this.evb.getIp(next);
		function next(val) {
			logo.hold = false; 
			logo.arglist.pop(); 
			logo.arglist.push(val);
		};
	}

}

prims['setip']  = {nargs: 1, fcn: function(n){riac.evb.url = 'ws://'+ String(n) + ':8899/websocket';}}
prims['connect']  = {nargs: 0, fcn: function(){riac.evb.connect();}}

prims['pinon'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'gpio_on'});}}
prims['pinoff'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'gpio_off'});}}
prims['fdrobot'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'forward'});}}
prims['bkrobot'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'back'});}}
prims['lrobot'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'left'});}}
prims['rrobot'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: String(n),cmd:'right'});}}

prims['analogin'] = {nargs: 0, fcn: function(n){riac.analog0(); return this.cfun}}
prims['scannetworks'] = {nargs: 0, fcn: function(n){riac.evb.getNetworks();}}
prims['getnetworks'] = {nargs: 0, fcn: function(n){return JSON.stringify(riac.evb.sensorState.wifiScan);}}
prims['setssid'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: {sta_ssid: String(n)},cmd:'setConfig'});}}
prims['setpass'] = {nargs: 1, fcn: function(n){riac.evb.send({arg: {sta_pass: String(n)},cmd:'setConfig'});}}
prims['getip'] = {nargs: 0, fcn: function(n){riac.getIp(); return this.cfun}}