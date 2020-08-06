var turtle;
var logo;
var commandCenter;
var procedures;
var extensions;
var riac;
var random;
var boxes = [0,0,0];
var variables = [{}];
var urlvars;
var filename = 'untitled';
var app = false;
var flushtime = 200;

window.onload = setup;

function setup(){
	var titleheight = window.outerHeight-window.innerHeight;
	window.resizeTo(1352,titleheight+670);
	window.addEventListener('resize',resize);
	frame.ondragover =  function (e) {self.allowDrop(e)}; 
	frame.ondrop = function (e) {self.handleDropFile(e)};
	cnvframe.onkeydown = handleCnvKeyDown;
	cnvframe.addEventListener('click',paneToggle);
	turtle = new Turtle();
	logo = new Logo();
	commandCenter = new CommandCenter();
	procedures = new Procedures();
	extensions = new Extensions();
	riac = new Riac();
	random = new Random();
	urlvars = extensions.getUrlVars();
	turtle.setup();
	extensions.setup();
	resize();
	if(location.href.split(':')[0]=='foo'){app=true; apponepane(); procs.value=clock; procedures.readProcs(); commandCenter.runLine('go')}
	else if(urlvars.gallery!=undefined) {onepane(); extensions.loadURL(urlvars.gallery, urlvars.filename);}
	else if(urlvars.app!=undefined) {app=true; apponepane(); turtle.hideTurtle(); extensions.loadURL('apps', urlvars.app);}
	window.requestAnimationFrame(ticker);
}

function ticker(){
	if(!logo.isDone()){
		var end = now()+flushtime;
		while(now()<end){
			if(logo.hold) break;
			if(logo.isDone()) break;
			logo.evalNext();
		}
		extensions.createDragImage();
		turtle.move();
	}
	window.requestAnimationFrame(ticker);
}

function allowDrop (evt) { 
	evt.preventDefault(); 
	evt.stopPropagation();
}

function handleDropFile (evt) { 
	evt.preventDefault(); 
	if (evt.stopPropagation) evt.stopPropagation();
	else evt.cancelBubble = true;
	var file = evt.dataTransfer.files[0];
	if(file==undefined) return;
	filename = file.name.split('.')[0];
	if(file.type=='text/plain'){
		var reader = new FileReader();
	  reader.onload = readprocs;
	  reader.readAsText(file);
	} else if(file.type=='image/png'){
		var reader = new FileReader();
	  reader.onload = readimage;
	  reader.readAsDataURL(file);
	} 

  function readprocs(){
		procs.value=reader.result; 
		procedures.readProcs();
  }

  function readimage(){
  	turtle.loadpng(reader.result, extensions.createDragImage);
  }
}

/////////////////////////////////////
// UI resize
/////////////////////////////////////

function resize (e) {
	if(app) apponepane()
	else if(cc.style.visibility=='hidden') onepane();
	else threepanes();
}

function threepanes(){
	var doch = getDocumentHeight();
	var docw = getDocumentWidth();
 	cc.style.visibility = 'visible';
 	procs.style.visibility = 'visible';
	var cnvh = doch-100-20-20;
	var cnvw = Math.round(cnvh*5/4);
	if((docw-cnvw-50)<260){
		cnvw = docw-260-50;
		cnvh = Math.round(cnvw*4/5);
	}
 	cnvframe.style.width = cnvw+'px';
 	canvas.style.width = cnvw+'px';
 	cnvframe.style.height = cnvh+'px';
 	canvas.style.height = cnvh+'px';
	cnvframe.style.left = (docw-cnvw-12)+'px';
	cnvframe.style.top = '10px';
 	cc.style.width = (cnvw-20) +'px';
 	cc.style.height = (doch-cnvh-20-20)+'px';
 	procs.style.width = (docw-cnvw-50)+'px';
 	procs.style.height = (doch-procs.offsetTop-22)+'px';
 	turtle.move();
}


function onepane(){
	var docw = getDocumentWidth();
	var doch = getDocumentHeight();
	var cnvh = doch-20;
	var cnvw = Math.round(cnvh*5/4);
	cnvframe.style.left = ((docw-cnvw-20)/2)+'px';
	cnvframe.style.top = '10px'
	cnvframe.style.width = cnvw+'px';
	canvas.style.width = cnvw+'px';
	cnvframe.style.height = cnvh+'px';
	canvas.style.height = cnvh+'px';
	cc.style.visibility = 'hidden';
	procs.style.visibility = 'hidden';
	turtle.move();
}

function apponepane(){
	var docw = getDocumentWidth();
	var doch = getDocumentHeight();
	var cnvw = docw;
	var cnvh = Math.floor(docw*4/5);
	cnvframe.style.left = '-1px';
	cnvframe.style.top = ((doch-cnvh)/2)+'px';
	cnvframe.style.width = cnvw+'px';
	canvas.style.width = cnvw+'px';
	cnvframe.style.height = cnvh+'px';
	canvas.style.height = cnvh+'px';
	cc.style.visibility = 'hidden';
	procs.style.visibility = 'hidden';
	turtle.move();
}

function paneToggle(e){
	if(!e.shiftKey) return;
	if(app) return;
	if(cc.style.visibility=='hidden') threepanes();
	else onepane();
}

function handleCnvKeyDown(e) {
 if(e.ctrlKey){
	if(e.keyCode==70) {e.preventDefault(); e.stopPropagation(); cc.focus();}
	if(e.keyCode==71) {e.preventDefault(); e.stopPropagation(); commandCenter.runLine('go');}
	if(e.keyCode==190) {commandCenter.insert('stopped!\n'); logo.reset([]);}
 }
}

function lprint(x){commandCenter.insert(x+'\n');}
