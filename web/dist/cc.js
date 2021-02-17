/////////////////////////
//
// CC Text Area
//
/////////////////////////

var runner;

class CommandCenter {

insert(str){
	var startpos = cc.selectionStart;
	var endpos = cc.selectionEnd;
	var t = cc.value;
	var before = t.substring(0,startpos);
	var after = t.substring(endpos);
	var oldtop = cc.scrollTop;
	cc.value = before+str;
	var halfscroll = cc.scrollHeight-cc.scrollTop-cc.offsetHeight;
	cc.value = before+str+after;
	cc.selectionStart = startpos+str.length;
	cc.selectionEnd = startpos+str.length;
	if(halfscroll>0) cc.scrollTop+=halfscroll;
	else cc.scrollTop = oldtop;
}

runLine(str){
	var line = Tokenizer.parse(str);
	logo.reset(line);
}

constructor(){
	cc.autocapitalize = 'off';
	cc.autocorrect = 'off';
	cc.autocomplete = 'off';
	cc.spellcheck = false;
	cc.onkeydown = function(e){handleCCKeyDown(e);}
	cc.focused = false;
	cc.onfocus = function(){cc.focused = true;};
	cc.onblur = function(){cc.focused = false;};
	cc.value = 'Welcome to Logo!\n';
	cc.selectionStart =  cc.value.length + 1 ; 
	cc.selectionEnd = cc.value.length + 1;
	cc.focus();
	

	function handleCCKeyDown(e) {
		var k = e.keyCode;
		if(k==13){
			if(e.shiftKey) insertcr(e);
			else handlecr(e);
		}
	 if(e.ctrlKey){
//	 	if(e.keyCode==70) {e.preventDefault(); e.stopPropagation(); procs.focus();}
	 	if(e.keyCode==71){e.preventDefault(); e.stopPropagation(); commandCenter.runLine('go');}
	  if(e.keyCode==190) {commandCenter.insert('stopped!\n'); logo.reset([]);}
	 }
	}

	function handlecr(e){
		var pos = cc.selectionStart;
		var t = cc.value;
		var start=t.lastIndexOf('\n', pos-1), end=t.indexOf('\n', pos);
		if(end<0) end=t.length;
		cc.selectionStart = end+1;
		if(end!=t.length) e.preventDefault();
		var str = t.substring(start+1,end);
		commandCenter.runLine(str);
	}

	function insertcr(e){
		e.preventDefault();
		var pos = cc.selectionStart;
		var t = cc.value;
		var before = t.substring(0,pos);
		var after = t.substring(pos);
		cc.value = before+'\n'+after;
		cc.selectionStart = pos+1;
		cc.selectionEnd = pos+1;
	}


}}
