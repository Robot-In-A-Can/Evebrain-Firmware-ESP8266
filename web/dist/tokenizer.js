/////////////////////////
//
// Tokenizer
//
/////////////////////////

class Tokenizer {

constructor(s){
this.str = s;
this.offset = 0;
}

static parse(s){return new Tokenizer(s).tokenize();}

tokenize(){
	var t = this;
	return readList();

function readList(){
	var a = new Array();
	skipSpace();
	while(true){
		if(eof()) break;
		var token = readToken();
		if(token==null) break;
		a.push(token);
	}
	return a;
}

function readToken(){
	var s = next();
	var n = Number(s);
	if(!isNaN(n)) return n;
	var first = s.charAt(0);
	if(first=="]") return null;
	if(first=="[") return readList();
	return s;
}

function next(){
	if(peekChar()=="'") return readString();
	var res='';
	if(delim()) res=nextChar();
	else {
		while(true){
			if(eof()) break;
			if(delim()) break;
			else res+=nextChar();
	}}
	skipSpace();
	return res;
}

function readString(){
	nextChar();
	var res="'";
	while (true){
		if(eof()) return res+"'";
		var c=nextChar();
		res+=c;
		if(c=="'") {skipSpace(); return res;}
	}
	return null;
}

function nextLine(){
	var res='';
	while (true){
		if(eof()) return res;
		var c=nextChar();
		if(c=='\n') return res;
		res+=c;
	}
}

function skipSpace(){
	while(true){
		if(eof()) return;
		var c = peekChar();
		if(c==';') {skipComment(); continue;}
		if(" \t\r\n,".indexOf(c)==-1) return;
		nextChar();
	}	
}

function skipComment(){
	while(true){
		var c = nextChar();
		if(eof()) return;
		if(c== '\n') return;
	}	
}

function delim(){return "()[] \t\r\n".indexOf(peekChar())!=-1;}
function peekChar(){return t.str.charAt(t.offset);}
function nextChar(){return t.str.charAt(t.offset++);}
function eof() {return t.str.length==t.offset;}	

}}

