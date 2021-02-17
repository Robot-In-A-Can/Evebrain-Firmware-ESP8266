class Extensions {

constructor(){
	this.ws = undefined;
	this.dragimage = undefined;
}

setup(){
	cnvframe.draggable = true;
	cnvframe.ondragstart = extensions.dragSave;
	this.createDragImage();
}

createDragImage(){
	var cnv = document.createElement("canvas");
  cnv.width = 87;
  cnv.height = 70;
	var ctx = cnv.getContext('2d');
	ctx.scale(.0625,.0625);
	ctx.drawImage(canvas, 0, 0);
	var img = new Image;
	img.src = cnv.toDataURL();
	extensions.dragimage = img;
}

dragSave(e){
	ImageData.setImageData(turtle.ctx, procs.value);
  e.dataTransfer.setDragImage(extensions.dragimage,43,35);
  e.dataTransfer.setData("DownloadURL", 'text/html:'+filename+'.png:'+canvas.toDataURL());
}

saveAs(name){
	var a = document.createElement('a');
	a.href = 'data:text/plain;charset=UTF-8,'+encodeURIComponent(procs.value);
	a.download = name+'.txt';
	frame.appendChild(a);
	a.click();
	frame.removeChild(a);
	filename = name;
}

saveImage(name){
	ImageData.setImageData(turtle.ctx, procs.value);
	var a = document.createElement('a');
  canvas.toBlob(next);

  function next(blob){
		a.href = URL.createObjectURL(blob);
		a.download = name+'.png';
		frame.appendChild(a);
		a.click();
		frame.removeChild(a);
	}
}

loadURL(gallery, filename){
	this.loadImage(gallery, filename, next);

	function next(){commandCenter.runLine('go');}
}

download(gallery, filename){
	logo.hold = true;
	this.loadImage(gallery, filename, function(){logo.hold=false;});

}

loadImage(gallery, filename, fcn){
	var req = new XMLHttpRequest();
	req.open('GET', '../artlogo-samples/'+gallery+'/'+filename+'.png'+'?timestap='+new Date().getTime(), true);
	req.onload = next1;
	req.responseType = 'blob';
	req.send();

	function next1(e){
		var fr = new FileReader();
		fr.onload = next2;
		fr.readAsDataURL(e.target.response);
	}

	function next2(d){turtle.loadpng(d.target.result, next3);}
	function next3(){if(fcn) fcn();}
}

downloadSamples(){
	var req = new XMLHttpRequest(); 
	req.open('GET', 'artsamples.zip?timestap='+new Date().getTime()); 
	req.onload = next;
	req.responseType = 'blob';
	req.send();
	
	function next(e){
//		console.log(e);
		var fr = new FileReader();
		fr.onload = next2;
		fr.readAsDataURL(e.target.response);
	}
	
	function next2(d){
		var a = document.createElement('a');
		a.href = d.target.result;
		a.download = 'artsamples.zip';
		frame.appendChild(a);
		a.click();
		frame.removeChild(a);
	}
}

upload(gallery,filename){
	ImageData.setImageData(turtle.ctx, procs.value);
	var req = new XMLHttpRequest();
	req.onreadystatechange= saved;
	req.open('PUT', '../artlogo-samples/post.php?gallery='+gallery+'&filename='+filename, true);
	req.setRequestHeader("Content-Type", 'text/plain');
	req.send(canvas.toDataURL().split(',')[1]);

	function saved(){
		if (req.readyState!=4) return;
		if (req.status!=200) return;
		commandCenter.insert('uploaded.\n');
	}
}

connect(addr){
	this.ws = new WebSocket('ws:'+addr+':2001');
	this.ws.onmessage = gotmessage;
	
	function gotmessage(m){
		var l = m.data.split(' ');
		var target = l[0];
		if(target!='host') return;
		var cmd = l[1];
		var arg = atob(l[2]);
		if(cmd=='print') commandCenter.insert('response: '+arg);
	}
}

wssend(who, cmd, arg){
	this.ws.send(who+' '+cmd+' '+btoa(arg));
}

getUrlVars(){
	if (window.location.href.indexOf('?') < 0) return [];
	var args = window.location.href.slice(window.location.href.indexOf('?') + 1);
	var vars = [], hash;
	var hashes = args.split('&');
	for(var i = 0; i < hashes.length; i++){
		hash = hashes[i].split('=');
		vars.push(hash[0]);
		vars[hash[0]] = hash[1];
	}
  return vars;
}

}


prims['saveas'] = {nargs: 1, fcn: function(n){extensions.saveAs(n);}}
prims['saveimage'] = {nargs: 1, fcn: function(n){extensions.saveImage(n);}}
prims['downloadsamples'] = {nargs: 0, fcn: function(){extensions.downloadSamples();}}
prims['upload'] = {nargs: 2, fcn: function(gallery,filename){extensions.upload(gallery,filename);}}
prims['download'] = {nargs: 2, fcn: function(gallery,filename){extensions.download(gallery,filename);}}

prims['connect'] = {nargs: 0, fcn: function(){extensions.connect(location.hostname);}}
prims['broadcast'] = {nargs: 3, fcn: function(who,cmd,arg){extensions.wssend(who,cmd,arg);}}
