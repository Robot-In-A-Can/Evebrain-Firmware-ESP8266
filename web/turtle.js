class Turtle {

constructor(){
this.cnvWidth = 700;
this.cnvHeight = 560;
this.img;
this.ctx;
this.xcor;
this.ycor;
this.element;
this.heading = 0;
this.color=0;
this.shade=50;
this.opacity = 1;
this.pendown= true;
this.pensize = 0;
this.size = 70;
this.font = 'sans-serif';
this.fontsize = 30;
this.dpi = 2;
this.zoom = 1;
this.snaps = {};

this.colors = [
	0xFF0000, 0xFF0D00, 0xFF1A00, 0xFF2600, 0xFF3300, 0xFF4000, 0xFF4D00, 0xFF5900, 0xFF6600, 0xFF7300, 
	0xFF8000, 0xFF8C00, 0xFF9900, 0xFFA600, 0xFFB300, 0xFFBF00, 0xFFCC00, 0xFFD900, 0xFFE600, 0xFFF200, 
	0xFFFF00, 0xE6FF00, 0xCCFF00, 0xB3FF00, 0x99FF00, 0x80FF00, 0x66FF00, 0x4DFF00, 0x33FF00, 0x1AFF00, 
	0x00FF00, 0x00FF0D, 0x00FF1A, 0x00FF26, 0x00FF33, 0x00FF40, 0x00FF4D, 0x00FF59, 0x00FF66, 0x00FF73, 
	0x00FF80, 0x00FF8C, 0x00FF99, 0x00FFA6, 0x00FFB3, 0x00FFBF, 0x00FFCC, 0x00FFD9, 0x00FFE6, 0x00FFF2, 
	0x00FFFF, 0x00F2FF, 0x00E6FF, 0x00D9FF, 0x00CCFF, 0x00BFFF, 0x00B3FF, 0x00A6FF, 0x0099FF, 0x008CFF, 
	0x0080FF, 0x0073FF, 0x0066FF, 0x0059FF, 0x004DFF, 0x0040FF, 0x0033FF, 0x0026FF, 0x001AFF, 0x000DFF, 
	0x0000FF, 0x0D00FF, 0x1A00FF, 0x2600FF, 0x3300FF, 0x4000FF, 0x4D00FF, 0x5900FF, 0x6600FF, 0x7300FF, 
	0x8000FF, 0x8C00FF, 0x9900FF, 0xA600FF, 0xB300FF, 0xBF00FF, 0xCC00FF, 0xD900FF, 0xE600FF, 0xF200FF, 
	0xFF00FF, 0xFF00E6, 0xFF00CC, 0xFF00B3, 0xFF0099, 0xFF0080, 0xFF0066, 0xFF004D, 0xFF0033, 0xFF001A, 
	0xFF0000];
	
}

setup(){
	var t = this;
	t.element = document.createElement('div');
	t.element.setAttribute ('class', 'turtle');
	cnvframe.appendChild(t.element);
	t.img = document.createElement('img');
	t.element.appendChild (t.img);
	t.img.src = 'turtle.svg';
	t.img.onload = imgLoaded;
	t.ctx = canvas.getContext('2d');
	canvas.width = t.cnvWidth*t.dpi;
	canvas.height = t.cnvHeight*t.dpi;
	t.ctx.scale(t.dpi,t.dpi);
	t.ctx.textBaseline="middle"; 
	t.clean();

	function imgLoaded(){
		t.img.width = t.size;
		t.img.height = t.size;
		t.element.style.width = t.size+'px';
		t.element.style.height = t.size+'px';
		t.move();
	}

}


/////////////////////////
//
// Turtle
//
/////////////////////////

	
forward(n){
	var t = this;
	if(t.pendown){
		t.ctx.beginPath();
		t.ctx.moveTo(t.xcor+t.cnvWidth/2, t.cnvHeight/2-t.ycor);
	}
	t.xcor+=n*sindeg(t.heading);
	t.ycor+=n*cosdeg(t.heading);
	if(t.pendown){
		var sx=t.xcor+t.cnvWidth/2, sy=t.cnvHeight/2-t.ycor;
		if(n>=.1)t.ctx.lineTo(sx,sy);
		else t.ctx.lineTo(sx, sy+.1);
		if(t.pensize!=0) t.ctx.stroke();
		if(t.fillpath) t.fillpath.push(function(){turtle.ctx.lineTo(sx,sy);});
	}
}

lineto(x,y){
	var t = this;
	if(t.pendown){
		t.ctx.beginPath();
		t.ctx.moveTo(t.xcor+t.cnvWidth/2, t.cnvHeight/2-t.ycor);
	}
	t.xcor = x;
	t.ycor = y;
	if(t.pendown){
		var sx=t.xcor+t.cnvWidth/2, sy=t.cnvHeight/2-t.ycor;
		if((x+y)>=.1)t.ctx.lineTo(sx,sy);
		else t.ctx.lineTo(sx, sy+.1);
		if(t.pensize!=0) t.ctx.stroke();
		if(t.fillpath) t.fillpath.push(function(){turtle.ctx.lineTo(sx,sy);});
	}
}

setxy(x,y){
	var t = this;
	t.xcor = x;
	t.ycor = y;
	var sx=t.xcor+t.cnvWidth/2, sy=t.cnvHeight/2-t.ycor;
	if(t.fillpath) t.fillpath.push(function(){turtle.ctx.moveTo(sx,sy);});
}

right(n){this.seth(this.heading+n);}
left(n){this.seth(this.heading-n);}
seth(a){this.heading=a; this.heading=this.heading.mod(360);}

arc(a,r){
	var t = this;
	if(a==0) return;	
	if(r==0) {t.seth(t.heading+a);}
	else if (a<0) leftArc(a,r);
	else rightArc(a,r);

	function rightArc(a,r){
		var sgn = r/Math.abs(r);
		var ar = Math.abs(r);
		var dx = ar*cosdeg(t.heading);
		var dy = ar*sindeg(t.heading);
		var cx = t.xcor+dx;
		var cy = t.ycor-dy;
		if(t.pendown){
			var sx=t.cnvWidth/2+cx, sy=t.cnvHeight/2-cy;
			var astart=rad(t.heading+180.0), aend=rad(t.heading+180+a*sgn);
			if((a%360)==0) aend+=.0001;
			var dir = r<0;
			t.ctx.beginPath();
			t.ctx.moveTo(t.xcor+t.cnvWidth/2, t.cnvHeight/2-t.ycor);
			t.ctx.arc(sx, sy, ar, astart, aend, dir); 
			if(t.pensize!=0) t.ctx.stroke();
			if(t.fillpath) t.fillpath.push(function(){turtle.ctx.arc(sx, sy, ar, astart,aend, dir);});
		}
		t.seth(t.heading+a*sgn);
		t.xcor = cx-ar*cosdeg(t.heading);
		t.ycor = cy+ar*sindeg(t.heading);
	} 

	function leftArc(a,r){
		var sgn = r/Math.abs(r);
		var ar = Math.abs(r);
		var dx = ar*cosdeg(t.heading);
		var dy = ar*sindeg(t.heading);
		var cx = t.xcor-dx;
		var cy = t.ycor+dy;
		if(t.pendown){
			var sx=t.cnvWidth/2+cx, sy=t.cnvHeight/2-cy;
			var astart=rad(t.heading), aend=rad(t.heading+a*sgn);
			var dir = r>=0;
			if((a%360)==0) aend+=.0001;
			t.ctx.beginPath();
			t.ctx.moveTo(t.xcor+t.cnvWidth/2, t.cnvHeight/2-t.ycor);
			t.ctx.arc(sx, sy, ar, astart,aend, dir); 
			if(t.pensize!=0) t.ctx.stroke();
			if(t.fillpath) t.fillpath.push(function(){turtle.ctx.arc(sx, sy, ar, astart,aend, dir);});
		}
		t.seth(t.heading+a*sgn);
		t.xcor = cx+ar*cosdeg(t.heading);
		t.ycor = cy-ar*sindeg(t.heading);
	}
}

showTurtle(){this.element.style.visibility = 'visible';}
hideTurtle(){this.element.style.visibility = 'hidden';}

/////////////////////////
//
// Pen
//
/////////////////////////

fillscreen(c,s){
	var oldcolor = this.color, oldshade=this.shade;
	if((typeof c)=='object') c = c[0];
	this.setCtxColorShade(c, s);
	this.ctx.fillRect(0,0,this.cnvWidth,this.cnvHeight);
	this.setCtxColorShade(this.color, this.shade);
}

clearscreen(){
	this.ctx.clearRect(0,0,this.cnvWidth,this.cnvHeight);
}

setcolor(c){
	if((typeof c)=='object'){this.color = c[0]; this.shade = c[1];}
	else this.color = c;
	this.setCtxColorShade(this.color, this.shade);
}

setshade(sh){
	this.shade=sh;
	this.setCtxColorShade(this.color, this.shade);
}

setpensize(ps){
	this.pensize = ps;
	this.ctx.lineWidth = Math.abs(this.pensize);
}

startfill(){
	this.fillpath = new Array();
	var sx=this.xcor+this.cnvWidth/2, sy=this.cnvHeight/2-this.ycor;
	this.fillpath.push(function(){turtle.ctx.moveTo(sx, sy);});
}

endfill(){
	if(!this.fillpath) return
	this.ctx.beginPath();
	for(var i in this.fillpath){
		if(i>2000) break;
		this.fillpath[i]();
	}
	this.ctx.globalAlpha = this.opacity;
	this.ctx.fill();
	this.ctx.globalAlpha = 1;
	this.fillpath = undefined;
}

setlinedash(l){
	this.ctx.setLineDash(l);
}

/////////////////////////
//
// Text
//
/////////////////////////

drawString(str){
	var t = this;
	t.ctx.save();
	turtle.ctx.translate(t.xcor+t.cnvWidth/2, t.cnvHeight/2-t.ycor);
	t.ctx.rotate(rad(t.heading));
	t.ctx.fillText(str,0,0);
	t.ctx.restore();	
}

setfont(f){
	this.font = f;
	this.ctx.font = this.fontsize+'px '+f;
}

setfontsize(s){
	this.fontsize = s;
	this.ctx.font = s+'px '+this.font;
}


/////////////////////////
//
//  Basic stuff
//
/////////////////////////

move(){
	var t = this;
	if(!t.img.complete) return;
	var img = t.element.firstChild;
	var dx = screenLeft();
	var dy = screenTop();
	var s = canvas.offsetHeight/t.cnvHeight*t.zoom;
	t.element.style.webkitTransform = 'translate('+dx+'px, '+ dy+ 'px) rotate(' + t.heading + 'deg)'+' scale('+s+','+s+')';
	t.element.left = dx; 
	t.element.top = dy; 

	function screenLeft() {return -img.width/2+(t.xcor+t.cnvWidth/2)*canvas.offsetWidth/t.cnvWidth;}
	function screenTop() {return -img.height/2+(t.cnvHeight/2-t.ycor)*canvas.offsetHeight/t.cnvHeight;}

}

clean(){
	var t = this;
	t.xcor=0, t.ycor=0, t.heading=0;
	t.setCtxColorShade(-9999, 98); // #FAFAFA
	t.ctx.fillRect(0,0,t.cnvWidth,t.cnvHeight);
	t.color=0, t.shade=50;
	t.setCtxColorShade(t.color, t.shade);
	t.pensize = 4;
	t.ctx.lineWidth=t.pensize;
	t.opacity = 1;
	t.pendown = true;
	t.fillpath = undefined;
	t.ctx.lineCap = 'round';
	t.ctx.lineJoin = 'round';
	t.font = 'sans-serif';
	t.fontsize = 30;
	t.ctx.font = '30px sans-serif';
	t.ctx.textAlign = 'center';
	t.ctx.setLineDash([]);	
	t.showTurtle();
}

/////////////////////////
//
// loader
//
/////////////////////////

loadpng(dataurl, fcn){
	var t = this;
	var ctx = this.ctx;
	var img = new Image;
	img.onload = drawImageToFit;
	img.src = dataurl;

	function drawImageToFit(){
		var code = readHiddenData();
		procs.value = (code=='bad sig') ? '' : code;
    var s = t.cnvWidth/img.naturalWidth;
		ctx.save();
		ctx.scale(s,s);
		ctx.drawImage(img, 0, 0);
		ctx.restore();	
		procedures.readProcs();
		if(fcn) fcn();
	}

	function readHiddenData(){
		var cnv = document.createElement("canvas");
    cnv.width = img.naturalWidth;
    cnv.height = img.naturalHeight;
		var ctx = cnv.getContext('2d');
		ctx.imageSmoothingEnabled = false;
		ctx.drawImage(img, 0, 0);
		return ImageData.getImageData(ctx);
	}
}

savesnap(name, fcn){
	var t = this;
	var ctx = this.ctx;
	var img = new Image;
	img.onload = fcn;
	img.src = canvas.toDataURL();
	this.snaps[name] = img;
}

drawsnap(name){
  var ctx = this.ctx;
  var img = this.snaps[name];
	var s = this.cnvWidth/img.naturalWidth;
	ctx.save();
	ctx.scale(s,s);
	ctx.drawImage(img, 0, 0);
	ctx.restore();	
}




/////////////////////////
//
// Low Level
//
/////////////////////////


setCtxColorShade(color, shade){
	var t = this;
	setCtxColor(mergeColorShade(color, shade));

	function mergeColorShade(color, shade){
		var sh = Math.abs(shade.mod(200));
		if(sh>100) sh = 200 - sh;
		if(color==-9999) return blend(0x000000, 0xffffff, sh/100);
		var c = colorFromNumber(color);
		if(sh==50) return c;
		else if (sh<50) return blend(c, 0x000000, (50-sh)/60);
		else return blend(c, 0xffffff, (sh-50)/53);
	}

	function colorFromNumber (c){
		var mc = c.mod(100);
		var ic = Math.floor(mc);
		var fract = mc - ic;
		return blend(t.colors[ic], t.colors[ic+1], fract);
	}

	function blend(a, b, s){
		var ar=(a>>16)&0xff, ag=(a>>8)&0xff, ab=a&0xff; 
		var br=(b>>16)&0xff, bg=(b>>8)&0xff, bb=b&0xff; 
		var rr = Math.round(ar*(1-s)+br*s);
		var rg = Math.round(ag*(1-s)+bg*s);
		var rb = Math.round(ab*(1-s)+bb*s);
		return (rr<<16)+(rg<<8)+rb;
	}

	function setCtxColor(c){
		var cc = '#'+(c+0x1000000).toString(16).substring(1);
		t.ctx.strokeStyle = cc;
	//  console.log ('ctx color:',cc);
		t.ctx.fillStyle = cc; 
	} 

}

}

function rad(a){return a*2*Math.PI/360;}
function deg(a){return a*360/(2*Math.PI);}
function sindeg(x){return Math.sin(x*2*Math.PI/360);}
function cosdeg(x){return Math.cos(x*2*Math.PI/360);}

