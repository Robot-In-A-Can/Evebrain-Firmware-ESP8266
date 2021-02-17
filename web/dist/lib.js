////////////////////////////////////////
// Library calls
///////////////////////////////////////

function rl() { window.location.reload(true);}
Number.prototype.mod = function(n) {return ((this%n)+n)%n;}
function last(l){return l[l.length-1];}
function getDocumentHeight(){return Math.max(document.body.clientHeight, document.documentElement.clientHeight);}
function getDocumentWidth(){return Math.max(document.body.clientWidth, document.documentElement.clientWidth);}

////////////////////////
//
// timer
//
/////////////////////////

var t0 = Date.now();
function resett(){t0 = Date.now();}
function timer(){return Date.now()-t0;}
function now(){return new Date().getTime();}

////////////////////////
//
// random
//
/////////////////////////

class Random {

constructor(){
this.seed = (new Date).getTime();
}


oneof(a,b){
	return this.nextRandomDouble()<.5 ? a : b;
}

pickRandom(a,b){
	var t = this;
	var ia = Math.floor(a);
	var ib = Math.floor(b);
	if((a==ia)&&(b==ib)) return a+(Math.floor(this.nextRandomDouble()*(b-a+1)));
	else return a+(this.nextRandomDouble()*(b-a));
}

nextRandomDouble(){
	var t = this;
	var r1 = nextRandom(26);
	var r2 = nextRandom(27);
	return (r1*Math.pow(2,27)+r2)/Math.pow(2,53);	
	
	function nextRandom(bits){
		var k = 0x5DEECE66D;
		var shift=1, mod=Math.pow(2,48);
		var part, res=0;
		for(var i=0;i<9;i++){
			part = (k%16)*t.seed%mod*shift;
			res+=part;
			k = Math.floor(k/16);
			shift*=16; mod/=16;
		}
		res+=11; res%=Math.pow(2,48);
		t.seed = res;
		return Math.floor(res/Math.pow(2,48-bits));
	}
}}

class ImageData {

static getImageData(ctx){
	var pixels = ctx.getImageData(0, 0, 500, 300).data;
	var x=0, y=0;
	var signature = readln(20);
	if(signature!='ArtLogo') return 'bad sig';
	return readln(100000);


	function readln(max){
		var res = '';
		var c, cksum=0;
		for (var i=0;i<max;i++) {
			c=rb();
			cksum += c;
			cksum &= 0xffff;
			if (c == 0xff) {
				if  (cksum != rb() + (rb() << 8)) res = "bad checksum";
				return decodeURIComponent(escape(res));
			}
			res = res + String.fromCharCode(c);
		}
		return 'bad readln';
	}

	function rb(){
		var res = 0;
		var idx = (x*4)+y*2000;
		res += (pixels[idx+2]&1);
		res += (pixels[idx+1]&1)<<1;
		res += (pixels[idx]&1)<<2;
		idx = (x*4)+(y+1)*2000;
		res += (pixels[idx+2]&1)<<3;
		res += (pixels[idx+1]&1)<<4;
		res += (pixels[idx]&1)<<5;
		idx = (x*4)+(y+2)*2000;
		res += (pixels[idx+2]&1)<<6;
		res += (pixels[idx+1]&1)<<7;
		x++;
		if (x==500) {x = 0; y+=3;}
		return res
	}
}

static setImageData(ctx, str){
	var imd = ctx.getImageData(0, 0, 500, 300);
	var pixels = imd.data;
	var x=0, y=0;
	println('ArtLogo');
	println(str);
	ctx.putImageData(imd,0,0);


	function println(str){
		var arr = explode(str);
		arr.push(0xff);
		var cksum = 0;
		for(var i in arr){
			wb(arr[i]);
			cksum+=arr[i];
		}
		wb(cksum&0xff);
		wb((cksum>>8)&0xff);
	}

	function wb(n){
		var idx = (x*4)+y*2000;
		pixels[idx+2]&=0xfe; pixels[idx+2]+=getbit(n,0);
		pixels[idx+1]&=0xfe; pixels[idx+1]+=getbit(n,1);
		pixels[idx]&=0xfe; pixels[idx]+=getbit(n,2);
		pixels[idx+3]=0xff;
		idx = (x*4)+(y+1)*2000;
		pixels[idx+2]&=0xfe; pixels[idx+2]+=getbit(n,3);
		pixels[idx+1]&=0xfe; pixels[idx+1]+=getbit(n,4);
		pixels[idx]&=0xfe; pixels[idx]+=getbit(n,5);
		pixels[idx+3]=0xff;
		idx = (x*4)+(y+2)*2000;
		pixels[idx+2]&=0xfe; pixels[idx+2]+=getbit(n,6);
		pixels[idx+1]&=0xfe; pixels[idx+1]+=getbit(n,7);
		pixels[idx+3]=0xff;
		x++;
		if (x==500) {x = 0; y+=3;}
	}

	function explode(str){
		var res = new Array();
		var ustr = unescape(encodeURIComponent(str));
		for(var i=0;i<ustr.length;i++) res.push(ustr.charCodeAt(i));
		return res;
	}

	function getbit(word, n) { return 1 & (word >> n); }
}


}

