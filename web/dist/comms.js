

Skip to content
Using Gmail with screen readers

1 of 987
comms.js
Inbox
x

Brian Silverman
Attachments
3:00 PM (0 minutes ago)
to me


Attachments area

class Comms{


constructor(){
	this.respfcn = undefined;
	this.respCount = 0;
	this.resp = [];
	this.respStr = '';
	this.serialID = undefined;
}

readsensor(n){
	logo.hold=true; 
	comms.adread(n,comms.gotsensor);	
}

gotsensor(x){
	var val = x[0]+256*x[1];
	logo.arglist.pop();
	logo.arglist.push(val);
	logo.hold = false;
}

readpin(n){
	logo.hold=true; 
	comms.dread(n,comms.gotpin);	
}

gotpin(x){
	logo.arglist.pop();
	logo.arglist.push(x[0]==0);
	logo.hold = false;
}

ping(fcn){
	var cmd = [0xff];
	this.sendReceive(cmd,1,fcn);
}

rb(addr,fcn){
	var cmd = [].concat(0xfe,this.twobytes(addr));
	this.sendReceive(cmd,1,fcn);
}

wb(addr,data){
	var cmd = [].concat(0xfd,this.twobytes(addr),1,data);
	this.sendl(cmd);
}

pin_on(n){this.sendl([0xe0+n]);}
pin_off(n){this.sendl([0xd0+n]);}
led_on(){this.sendl([0xef]);}
led_off(){this.sendl([0xdf]);}

adread(n,fcn){this.sendReceive([0xc0+n],2,fcn);}
dread(n,fcn){this.sendReceive([0xc0+n],1,fcn);}

redraw(l){this.sendl([].concat(0xb0,l));}
dotbrightness(b){this.sendl([0xb1,b]);}


twobytes(n){return [n&0xff, (n>>8)&0xff];}
	
sendReceive(l, n, fcn){
	this.respfcn = fcn;
	this.resp = [];
	this.respCount = n;
	chrome.serial.send(this.serialID, new Uint8Array(l), ()=>{});
}

sendl(l){
	var t = this;
	chrome.serial.send(t.serialID, new Uint8Array(l), next);

	function next(e){
		if(e.error!=undefined) console.log('send error:',l,e);
	}
}


openSerialPort(){
	var t = this;
	chrome.serial.getDevices(gotDevices);


	function gotDevices(devices){
		for(var i in devices){
			var d = devices[i];
//			console.log(d, validDevice(d));
			if(!validDevice(d)) continue;
			chrome.serial.connect(d.path, {bitrate: 115200}, connected);
			return;
		}
	}

	function validDevice(d){
		if(d.path.indexOf('cu.usbmodem')!=-1) return true;
		if(d.path.indexOf('cu.usbserial')!=-1) return true;
		if(d.path.indexOf('ttyACM')!=-1) return true;
		if(d.path.indexOf('ttyUSB')!=-1) return true;
		if((d.productId==0x0204)&&(d.vendorId==0x0d28)) return true;
		return false;
	}


	function connected(r){
//		console.log(r);
		if(t.serialID==undefined) chrome.serial.onReceive.addListener(onrecc);
		if(t.serialID==undefined) chrome.serial.onReceiveError.addListener(console.log);
		t.serialID = r.connectionId;
		console.log('connected');

		function onrecc(r){
			var l = Array.from(new Uint8Array(r.data));
			for(var i in l) gotChar(l[i]);
		}

		function gotChar(c){
			if(t.respCount==0) return;
			else {
				t.resp.push(c);
				if(t.respCount>t.resp.length) return;
				if(t.respfcn){
					t.respfcn(t.resp);
					t.respCount = 0;
					t.resp = [];
				}
			}
		}

	}
}

}

prims['openport'] = {nargs: 0, fcn: function(){comms.openSerialPort();}}

prims['ledon'] = {nargs: 0, fcn: function(){comms.led_on(); this.mwait(1);}}
prims['ledoff'] = {nargs: 0, fcn: function(){comms.led_off(); this.mwait(1);}}

prims['on3'] = {nargs: 0, fcn: function(){comms.pin_on(3); this.mwait(1);}}
prims['off3'] = {nargs: 0, fcn: function(){comms.pin_off(3); this.mwait(1);}}
prims['on4'] = {nargs: 0, fcn: function(){comms.pin_on(4); this.mwait(1);}}
prims['off4'] = {nargs: 0, fcn: function(){comms.pin_off(4); this.mwait(1);}}
prims['on5'] = {nargs: 0, fcn: function(){comms.pin_on(5); this.mwait(1);}}
prims['off5'] = {nargs: 0, fcn: function(){comms.pin_off(5); this.mwait(1);}}
prims['on6'] = {nargs: 0, fcn: function(){comms.pin_on(6); this.mwait(1);}}
prims['off6'] = {nargs: 0, fcn: function(){comms.pin_off(6); this.mwait(1);}}
prims['on7'] = {nargs: 0, fcn: function(){comms.pin_on(7); this.mwait(1);}}
prims['off7'] = {nargs: 0, fcn: function(){comms.pin_off(7); this.mwait(1);}}

prims['sensor0'] = {nargs: 0, fcn: function(){comms.readsensor(0); return this.cfun;}}
prims['sensor1'] = {nargs: 0, fcn: function(){comms.readsensor(1); return this.cfun;}}
prims['sensor2'] = {nargs: 0, fcn: function(){comms.readsensor(2); return this.cfun;}}
prims['sensor3'] = {nargs: 0, fcn: function(){comms.readsensor(3); return this.cfun;}}
prims['sensor4'] = {nargs: 0, fcn: function(){comms.readsensor(4); return this.cfun;}}
prims['sensor5'] = {nargs: 0, fcn: function(){comms.readsensor(5); return this.cfun;}}

prims['connected8'] = {nargs: 0, fcn: function(){comms.readpin(8); return this.cfun;}}
prims['connected9'] = {nargs: 0, fcn: function(){comms.readpin(9); return this.cfun;}}
prims['connected10'] = {nargs: 0, fcn: function(){comms.readpin(10); return this.cfun;}}
prims['connected11'] = {nargs: 0, fcn: function(){comms.readpin(11); return this.cfun;}}
prims['connected12'] = {nargs: 0, fcn: function(){comms.readpin(12); return this.cfun;}}

comms.txt
Displaying comms.txt.