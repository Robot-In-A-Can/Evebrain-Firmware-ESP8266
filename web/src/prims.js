var constants = {
	black: '-9999&0', white: '-9999&100', red: '0&50',  green: '30&50', blue: '70&50',
	cyan: '50&50', magenta: '90&50', yellow: '20&50', orange: '14&50'
} 

var prims = {};

prims['repeat'] = {nargs: 2, flow: true, fcn: function(a,b){this.repeat(a,b);}}
prims['forever'] = {nargs: 1, flow: true, fcn: function(a){this.loop(a);}}
prims['loop'] = {nargs: 1, flow: true, fcn: function(a){this.loop(a);}}
prims['if'] = {nargs: 2, flow: true, fcn: function(a,b){this.logo_if(this.getbool(a),b);}}
prims['ifelse'] = {nargs: 3, flow: true, fcn: function(a,t,f){this.logo_ifelse(this.getbool(a),t,f);}}
prims['run'] = {nargs: 1, flow: true, fcn: function(l){this.logo_run(l);}}

prims['stop'] = {nargs: 0, flow: true, fcn: function(){this.procOutput(this);}}
prims['output'] = {nargs: 1, flow: true, fcn: function(x){return this.procOutput(this,x);}}
prims['wait'] = {nargs: 1, fcn: function(x){this.mwait(100*this.getnum(x));}}

prims['+'] = {nargs: 2, priority: -1, fcn: function(a,b){return a+b;}}
prims['-'] = {nargs: 2, priority: -1, fcn: function(a,b){return a-b;}}
prims['*'] = {nargs: 2, priority: -2, fcn: function(a,b){return a*b;}}
prims['/'] = {nargs: 2, priority: -2, fcn: function(a,b){return a/b;}}
prims['='] = {nargs: 2, priority: -2, fcn: function(a,b){return logo.equals(a,b);}}
prims['!='] = {nargs: 2, priority: -2, fcn: function(a,b){return !logo.equals(a,b);}}
prims['>'] = {nargs: 2, priority: -2, fcn: function(a,b){return a>b;}}
prims['<'] = {nargs: 2, priority: -2, fcn: function(a,b){return a<b;}}
prims['remainder'] = {nargs: 2, fcn: function(a,b){return this.getnum(a).mod(this.getnum(b));}}
prims['round'] = {nargs: 1, fcn: function(a){return Math.round(this.getnum(a));}}
prims['int'] = {nargs: 1, fcn: function(a){return Math.floor(this.getnum(a));}}
prims['minus'] = {nargs: 1, fcn: function(a){return -a;}}
prims['sin'] = {nargs: 1, fcn: function(a){return sindeg(this.getnum(a));}}
prims['cos'] = {nargs: 1, fcn: function(a){return cosdeg(this.getnum(a));}}
prims['sqrt'] = {nargs: 1, fcn: function(a){return Math.sqrt(this.getnum(a));}}
prims['random2'] = {nargs: 2, fcn: function(a,b){return random.pickRandom(a,b);}}
prims['oneof'] = {nargs: 2, fcn: function(a,b){return random.oneof(a,b);}}

prims['sum'] = {nargs: 2, fcn: function(a,b){return a+b;}}
prims['product'] = {nargs: 2, fcn: function(a,b){return a*b;}}

prims['se'] = {nargs: 2, fcn: function(a,b){return [].concat(a,b);}}
prims['word'] = {nargs: 2, fcn: function(a,b){return logo.word(a,b);}}
prims['first'] = {nargs: 1, fcn: function(a){return logo.first(a);}}
prims['butfirst'] = {nargs: 1, fcn: function(a){return logo.butfirst(a);}}
prims['bf'] = {nargs: 1, fcn: function(a){return logo.butfirst(a);}}
prims['last'] = {nargs: 1, fcn: function(a){return logo.last(a);}}
prims['bl'] = {nargs: 1, fcn: function(a){return logo.butlast(a);}}
prims['fput'] = {nargs: 2, fcn: function(a,b){var res = [].concat(this.getlist(b)); res.unshift(a); return res;}}
prims['lput'] = {nargs: 2, fcn: function(a,b){var res = [].concat(this.getlist(b)); res.push(a); return res;}}
prims['count'] = {nargs: 1, fcn: function(a){return logo.count(a);}}
prims['item'] = {nargs: 2, fcn: function(n,l){return logo.item(n,l);}}
prims['nth'] = {nargs: 2, fcn: function(n,l){return this.getlist(l)[this.getnum(n)];}}
prims['setnth'] = {nargs: 3, fcn: function(n,l,d){this.getlist(l)[this.getnum(n)]=d;}}
prims['member?'] = {nargs: 2, fcn: function(x,l){return logo.member(x,l);}}
prims['empty?'] = {nargs: 1, fcn: function(l){return l.length==0;}}
prims['pick'] = {nargs: 1, fcn: function(l){return l[random.pickRandom(0,this.getlist(l).length-1)];}}

prims['print'] = {nargs: 1, fcn: function(x){lprint(logo.printstr(x));}}

prims['clean'] = {nargs: 0, fcn: function(n){turtle.clean();}}
prims['forward'] = {nargs: 1, fcn: function(n){turtle.forward(this.getnum(n));}}
prims['fd'] = {nargs: 1, fcn: function(n){turtle.forward(this.getnum(n));}}
prims['back'] = {nargs: 1, fcn: function(n){turtle.forward(this.getnum(-n));}}
prims['bk'] = {nargs: 1, fcn: function(n){turtle.forward(this.getnum(-n));}}
prims['right'] = {nargs: 1, fcn: function(n){turtle.right(this.getnum(n));}}
prims['rt'] = {nargs: 1, fcn: function(n){turtle.right(this.getnum(n));}}
prims['left'] = {nargs: 1, fcn: function(n){turtle.right(this.getnum(-n));}}
prims['lt'] = {nargs: 1, fcn: function(n){turtle.right(this.getnum(-n));}}
prims['setheading'] = {nargs: 1, fcn: function(n){turtle.seth(this.getnum(n));}}
prims['seth'] = {nargs: 1, fcn: function(n){turtle.seth(this.getnum(n));}}
prims['setxy'] = {nargs: 2, fcn: function(x,y){turtle.setxy(this.getnum(x),this.getnum(y));}}
prims['lineto'] = {nargs: 2, fcn: function(x,y){turtle.lineto(this.getnum(x),this.getnum(y));}}
prims['arc'] = {nargs: 2, fcn: function(a,r){turtle.arc(this.getnum(a),this.getnum(r));}}

prims['fillscreen'] = {nargs: 2, fcn: function(c,s){turtle.fillscreen(this.getcolor(c),s);}}
prims['clearscreen'] = {nargs: 0, fcn: function(c,s){turtle.clearscreen();}}
prims['setcolor'] = {nargs: 1, fcn: function(n){turtle.setcolor(this.getcolor(n));}}
prims['setc'] = {nargs: 1, fcn: function(n){turtle.setcolor(this.getcolor(n));}}
prims['setshade'] = {nargs: 1, fcn: function(n){turtle.setshade(n);}}
prims['setsh'] = {nargs: 1, fcn: function(n){turtle.setshade(n);}}
prims['setpensize'] = {nargs: 1, fcn: function(n){turtle.setpensize(n);}}
prims['setps'] = {nargs: 1, fcn: function(n){turtle.setpensize(n);}}
prims['pendown'] = {nargs: 0, fcn: function(n){turtle.pendown=true;}}
prims['pd'] = {nargs: 0, fcn: function(n){turtle.pendown=true;}}
prims['penup'] = {nargs: 0, fcn: function(n){turtle.pendown=false;}}
prims['pu'] = {nargs: 0, fcn: function(n){turtle.pendown=false;}}
prims['startfill'] = {nargs: 0, fcn: function(){turtle.startfill();}}
prims['endfill'] = {nargs: 0, fcn: function(){turtle.endfill();}}
prims['setopacity'] = {nargs: 1, fcn: function(n){turtle.opacity=this.getnum(n)/100;}}

prims['drawtext'] = {nargs: 1, fcn: function(str){turtle.drawString(this.printstr(str));}}
prims['textalign'] = {nargs: 1, fcn: function(str){logo.textAlign(str);}}
prims['setfont'] = {nargs: 1, fcn: function(f){turtle.setfont(f);}}
prims['setfontsize'] = {nargs: 1, fcn: function(s){turtle.setfontsize(s);}}
prims['setlinestyle'] = {nargs: 1, fcn: function(l){turtle.setlinedash(l);}}

prims['xcor'] = {nargs: 0, fcn: function(n){return turtle.xcor;}}
prims['ycor'] = {nargs: 0, fcn: function(n){return turtle.ycor;}}
prims['heading'] = {nargs: 0, fcn: function(n){return turtle.heading;}}
prims['color'] = {nargs: 0, fcn: function(n){return turtle.color;}}
prims['shade'] = {nargs: 0, fcn: function(n){return turtle.shade;}}
prims['pensize'] = {nargs: 0, fcn: function(n){return turtle.pensize;}}
prims['opacity'] = {nargs: 0, fcn: function(n){return 100*turtle.opacity;}}

prims['hideturtle'] = {nargs: 0, fcn: function(n){turtle.hideTurtle();}}
prims['ht'] = {nargs: 0, fcn: function(n){turtle.hideTurtle();}}
prims['showturtle'] = {nargs: 0, fcn: function(n){turtle.showTurtle();}}
prims['st'] = {nargs: 0, fcn: function(n){turtle.showTurtle();}}

prims['snapimage'] = {nargs: 1, fcn: function(n){logo.startHold(); turtle.savesnap(n, logo.endHold);}}
prims['drawsnap'] = {nargs: 1, fcn: function(n){logo.notUndefined(turtle.snaps[n], n); turtle.drawsnap(n);}}

prims['flushtime'] = {nargs: 1, fcn: function(n){flushtime=this.getnum(n);}}
prims['seed'] = {nargs: 0, fcn: function(n){return random.seed;}}
prims['setseed'] = {nargs: 1, fcn: function(n){random.seed=this.getnum(n);}}

prims['( '] = {nargs: 1, fcn: function(x){this.evline.shift(); return x;}}
prims['se '] = {nargs: 'ipm', fcn: function(){return Array.prototype.slice.call(arguments);}}


prims['storeinbox1'] = {nargs: 1, fcn: function(n){boxes[0]=n;}}
prims['box1'] = {nargs: 0, fcn: function(){return boxes[0];}}
prims['storeinbox2'] = {nargs: 1, fcn: function(n){boxes[1]=n;}}
prims['box2'] = {nargs: 0, fcn: function(){return boxes[1];}}
prims['storeinbox3'] = {nargs: 1, fcn: function(n){boxes[2]=n;}}
prims['box3'] = {nargs: 0, fcn: function(){return boxes[2];}}

prims['resett'] = {nargs: 0, fcn: function(n){resett();}}
prims['timer'] = {nargs: 0, fcn: function(){return timer();}}
prims['time'] = {nargs: 0, fcn: function(){return logo.time();}}
prims['hours'] = {nargs: 0, fcn: function(){return logo.hours();}}
prims['minutes'] = {nargs: 0, fcn: function(){return logo.minutes();}}
prims['seconds'] = {nargs: 0, fcn: function(){return logo.seconds();}}
prims['2digit'] = {nargs: 1, fcn: function(n){return logo.twoDigit(n);}}
prims['clockspeed'] = {nargs: 1, fcn: function(n){this.clockspeed=this.getnum(n);}}
prims['scale'] = {nargs: 2, fcn: function(n,l){return logo.scale(this.getnum(n),this.getlist(l));}}

prims['make'] = {nargs: 2, fcn: function(a,b){this.setValue(a,b);}}
prims['local'] = {nargs: 1, fcn: function(a,b){this.makeLocal(a);}}

