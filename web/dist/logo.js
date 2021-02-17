/////////////////////////
//
// Logo Evaluator
//
/////////////////////////

class Logo {

constructor(){
this.evline = [];
this.cfun = undefined;
this.arglist = [];
this.priority = 0;
this.stack = [];
this.frame = [];
this.locals = [{}];
this.hold = false;
this.timeout = undefined;
this.clockspeed = 1;
}

reset(l){
this.evline = l;
this.cfun = undefined;
this.arglist = [];
this.priority = 0;
this.stack = [];
this.frame = [];
this.locals = [last(this.locals)];
this.hold = false;
if(this.timeout!=undefined) clearTimeout(this.timeout);
this.timeout = undefined;
}

evalNext(){
	var t = this;
	try {
		if(t.cfun){
			if(t.arglist.length==prims[t.cfun].nargs){funcall(); return;}
			if((prims[t.cfun].nargs=='ipm')&&(t.evline[0]==')')) {t.evline.shift(); funcall(); return}
		}
		if(t.evline.length==0){
			if(t.cfun==undefined) t.evalEOL(); 
			else throw 'not enough inputs to '+t.cfun;
			return;
		}
		var token = t.evline.shift();
		if ((typeof token)=='string') evalString();
		else t.pushResult(token);
	} 
	catch(e){
		lprint(e);
		t.stack = [];
		t.evline = [];
	}

	function evalString(){
		if(token.substring(0,1)==':') t.pushResult(t.getValue(token.substring(1)));
		else if(token.substring(0,1)=='"') t.pushResult(token.substring(1));
		else if((token.substring(0,1)=="'")&&(token.slice(-1)=="'")) t.pushResult(token.substring(1,token.length-1));
		else if(constants[token]) t.pushResult(constants[token]);
		else {	
			if(token=='(') handleParend();
			if(prims[token]==undefined) throw "I don't know how to "+token; 
			t.stack.push(t.cfun);
			t.stack.push(t.arglist);
			t.stack.push(t.priority);
			t.cfun = token;
			t.arglist = [];
			t.priority = 0;
		}

		function handleParend(){
			if(t.evline[0]=='se'){t.evline.shift(); token = 'se ';}
			else token = '( ';
		}
	}

	function funcall(){
		if(prims[t.cfun].flow) prims[t.cfun].fcn.apply(t, t.arglist);
		else if((typeof prims[t.cfun].fcn)=='function') primCall();
		else if((typeof prims[t.cfun].fcn)=='string') procCall();
	}


	function primCall(){
		var arglist=t.arglist;
		var prim = t.cfun;
		var res = prims[t.cfun].fcn.apply(t, arglist);
		t.priority = t.stack.pop();
		t.arglist = t.stack.pop();
		t.cfun = t.stack.pop();
		if((res==undefined)&&(t.cfun!=undefined)) throw prim+" didn't output to "+t.cfun;
		t.pushResult(res);
	}

	function procCall(){
		var cfun = t.cfun, arglist=t.arglist;
		t.stack.push(t.evline);
		t.stack.push(t.frame);
		t.frame = [].concat(t.stack);
		bindArgs();
		t.evalLine(prims[cfun].parsed, t.procOutput);

		function bindArgs(){
			var bindings = {};
			var inputs = prims[cfun].inputs;
			for(var i in inputs) bindings[inputs[i]]=arglist[i];
			t.locals.unshift(bindings);
		}
	}
}

pushResult(res){
	var t = this;
	if(res==undefined) return;
	if(t.cfun==undefined) throw "you don't say what to do with "+t.printstr(res);
	if (isInfixNext()) infixCall(res);
	else t.arglist.push(res);

	function infixCall(arg){
		t.stack.push(t.cfun);
		t.stack.push(t.arglist);
		t.stack.push(t.priority);
		t.cfun = t.evline.shift();
		t.arglist = [arg];
		t.priority = prims[t.cfun].priority;
	}

	function isInfixNext(){
		if(t.evline.length==0) return false;
		var token = t.evline[0];
		if(prims[token]==undefined) return false;
		if(prims[token].priority==undefined) return false;
		return(prims[token].priority<t.priority);
	}
}

getValue(name){
	for(var i in this.locals) {
		if(this.locals[i][name]!=undefined) return this.locals[i][name];
	}
	throw name+' has no value';
}

setValue(name, value){
	var t = this;
	for(var i in t.locals) {
		if(t.locals[i][name]!=undefined){
		 t.locals[i][name]=value;
		 return;
		}
	}
	t.locals[t.locals.length-1][name] = value;
}

makeLocal(name){this.locals[0][name]=0;}

procOutput(t, x){
	if(t.frame.length==0) {
		if(x!=undefined) throw "output can only be used in a procedure.";
		this.reset([]); 
		return;}
	t.stack = t.frame;
	t.frame = t.stack.pop();
	t.evline = t.stack.pop();
	t.priority = t.stack.pop();
	t.arglist = t.stack.pop();
	t.cfun = t.stack.pop();
	t.locals.shift();
	t.pushResult(x);
}

evalLine(l,next){
	var t = this;
	t.stack.push(t.cfun);
	t.stack.push(t.arglist);
	t.stack.push(t.evline);
	t.stack.push(next);
	t.cfun = undefined;
	t.arglist = [];
	t.evline = [].concat(l);
}

evalEOL(){
	var t = this;
	if(t.stack.length==0) return;
	var next = t.stack.pop();
	t.evline = t.stack.pop();
	t.arglist = t.stack.pop();
	t.cfun = t.stack.pop();
	next(t);
}

flowEnd(){
	var t = this;
	var prim = t.cfun;
	t.priority = t.stack.pop();
	t.arglist = t.stack.pop();
	t.cfun = t.stack.pop();
	if(t.cfun!=undefined) throw prim+" didn't output to "+t.cfun;
}

repeat(n,l){
	n = Math.round(this.getnum(n));
	this.stack.push(n);
	this.stack.push(l);
	repeatAgain(this);

	function repeatAgain(t){
		var l = t.stack.pop();
		var n = t.stack.pop();
		if(n<=0) {t.flowEnd(); return;}
		t.stack.push(--n);
		t.stack.push(l);
		t.evalLine(l,repeatAgain);
	}
}

loop(l){
	this.stack.push(l);
	loopAgain(this);

	function loopAgain(t){
		var l = t.stack.pop();
		t.stack.push(l);
		t.evalLine(l,loopAgain);
	}
}

logo_run(l){
	var t = this;
	t.evalLine(l, next);
	
	function next(){
		t.flowEnd();
	}
}

logo_if(b,l){
	var t = this;
	if(!b) t.flowEnd();
	else t.evalLine(l, next);
	
	function next(){
		t.flowEnd();
	}
}

logo_ifelse(b,l1,l2){
	var t = this;
	if(b) t.evalLine(l1, next);
	else t.evalLine(l2, next);
	
	function next(){
		t.flowEnd();
	}
}

item(n,l){
	n = this.getnum(n);
	if((typeof l)=='object') return l[n-1];
	return String(l).substring(n-1,n);
}

first(l){
	if((typeof l)=='object') return l[0];
	return String(l).substring(0,1);
}

butfirst(l){
	if((typeof l)=='object') return l.slice(1);
	return String(l).substring(1);
}

last(l){
	if((typeof l)=='object') return l[l.length-1];
	return String(l).substring(String(l).length-1);
}

butlast(l){
	if((typeof l)=='object') return l.slice(0,-1);
	return String(l).substring(0,String(l).length-1);
}

count(l){
	if((typeof l)=='object') return l.length;
	return String(l).length;
}

word(a,b){
	if((typeof a)=='object') a=a.join(' ');
	if((typeof b)=='object') b=b.join(' ');
	return String(a)+String(b);
}

member(x,l){
	if((typeof l)=='object'){
		for(var i=0;i<l.length;i++) {if(this.equals(x,l[i])) return true;}
		return false;
	}
	return String(l).indexOf(x)!=-1;
}

time(){
	var now = this.getDate(); 
	var hour = now.getHours();
	if(hour==0) hour=12;
	if(hour>12) hour-=12;
	return [hour, now.getMinutes(), now.getSeconds()];
}

hours(){
	var now = this.getDate(); 
	var hour = now.getHours();
	var second = now.getSeconds();
	var minute = now.getMinutes();
	if(hour==0) hour=12;
	if(hour>12) hour-=12;
	hour+=minute/60;
	hour+=second/3600;
	return Math.floor(hour*100)/100;
}

minutes(){
	var now = this.getDate(); 
	var minute = now.getMinutes();
	var second = now.getSeconds();
	minute+=second/60;
	return Math.floor(minute*100)/100;
}

seconds(){
	var now = this.getDate(); 
	var second = now.getSeconds();
	var millis = now.getMilliseconds();
	second+=millis/1000;
	return Math.floor(second*100)/100;
}

getDate(){
	return new Date(t0+(now()-t0)*this.clockspeed);
}

twoDigit(n){
	n = Math.floor(this.getnum(n));
	n = n.mod(100)+100;
	return String(n).substring(1);
}

scale(n,l){
	var len = l.length;
	for(var i=0;i<len-1;i++){
		if(l[0][0]==n) return l[0][1];
		if(l[1][0]>n){
			var la = l[0];
			var lb = l[1];
			var fract = (n-la[0])/(lb[0]-la[0]);
			return la[1]+fract*(lb[1]-la[1]);
		}
		l = l.slice(1);
	}
	return n;
}

textAlign(str){
	if(['center','left','right'].indexOf(str)>-1) turtle.ctx.textAlign = str;
	else throw this.cfun+" doesn't like "+this.printstr(str)+' as input'; 
}


mwait(n){
	if(n<=0) return;
	this.hold=true;
	this.timeout = setTimeout(function(){logo.timeout=undefined; logo.hold=false;}, n);
}

printstr(x){
	var type = typeof x;
	if(Number.isInteger(x)) return String(x);
	if(type=='number') return String(Math.round(x*10000)/10000);
	if(type=='object'){
		var res = '';
		for(var i in x){res+= this.printstr(x[i]); res+=' ';}
		return '['+res.substring(0,res.length-1)+']';
	}
	else return String(x);
}

getnum(x){
	var n = Number(x);
	if(isNaN(n)||(String(x)=='false')||(String(x)=='true')) throw this.cfun+" doesn't like "+this.printstr(x)+' as input'; 
	return n;
}

getlist(x){
	if((typeof x) == 'object') return x;
	throw this.cfun+" doesn't like "+this.printstr(x)+' as input'; 
}

getbool(x){
	if(String(x)=='false') return false;
	if(String(x)=='true') return true;
	throw this.cfun+" doesn't like "+this.printstr(x)+' as input'; 
}

getcolor(x){
	var type = typeof x;
	if(type=='object') throw this.cfun+" doesn't like "+this.printstr(x)+' as input'; 
	if(type=='number') return x;
	var l = x.split('&');
	if(l.length==1) return Number(x);
	return [Number(l[0]), Number(l[1])];
}

notUndefined(x,n){
	if(x!=undefined) return;
	throw this.cfun+" doesn't like "+this.printstr(n)+' as input'; 
}


equals(a,b){
	if((typeof a)!='object') return a.toString()==b.toString();
	if((typeof b)!='object') return a.toString()==b.toString();
	if(a.length!=b.length) return false;
	for(var i=0;i<a.length;i++){if(!this.equals(a[i],b[i])) return false;}
	return true;
}

startHold(){logo.hold = true;}
endHold(){logo.hold = false;}
stackPeek(n){return this.stack[this.stack.length-n-1];}
isDone(){return (this.stack.length==0)&&(this.evline.length==0);}
}

