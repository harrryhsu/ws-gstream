(()=>{var e={465:e=>{function t(e){this.gl=e,this.texture=e.createTexture(),e.bindTexture(e.TEXTURE_2D,this.texture),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_MAG_FILTER,e.LINEAR),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_MIN_FILTER,e.LINEAR),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_WRAP_S,e.CLAMP_TO_EDGE),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_WRAP_T,e.CLAMP_TO_EDGE)}t.prototype.bind=function(e,t,r){var i=this.gl;i.activeTexture([i.TEXTURE0,i.TEXTURE1,i.TEXTURE2][e]),i.bindTexture(i.TEXTURE_2D,this.texture),i.uniform1i(i.getUniformLocation(t,r),e)},t.prototype.fill=function(e,t,r){var i=this.gl;i.bindTexture(i.TEXTURE_2D,this.texture),i.texImage2D(i.TEXTURE_2D,0,i.LUMINANCE,e,t,0,i.LUMINANCE,i.UNSIGNED_BYTE,r)},e.exports={renderFrame:function(e,t,r,i,a,o){e.viewport(0,0,e.canvas.width,e.canvas.height),e.clearColor(0,0,0,0),e.clear(e.COLOR_BUFFER_BIT),e.y.fill(a,o,t),e.u.fill(a>>1,o>>1,r),e.v.fill(a>>1,o>>1,i),e.drawArrays(e.TRIANGLE_STRIP,0,4)},setupCanvas:function(e,r){var i=e.getContext("webgl")||e.getContext("experimental-webgl");if(!i)return i;var a=i.createProgram(),o=["attribute highp vec4 aVertexPosition;","attribute vec2 aTextureCoord;","varying highp vec2 vTextureCoord;","void main(void) {"," gl_Position = aVertexPosition;"," vTextureCoord = aTextureCoord;","}"].join("\n"),n=i.createShader(i.VERTEX_SHADER);i.shaderSource(n,o),i.compileShader(n);var s=["precision highp float;","varying lowp vec2 vTextureCoord;","uniform sampler2D YTexture;","uniform sampler2D UTexture;","uniform sampler2D VTexture;","const mat4 YUV2RGB = mat4","("," 1.1643828125, 0, 1.59602734375, -.87078515625,"," 1.1643828125, -.39176171875, -.81296875, .52959375,"," 1.1643828125, 2.017234375, 0, -1.081390625,"," 0, 0, 0, 1",");","void main(void) {"," gl_FragColor = vec4( texture2D(YTexture, vTextureCoord).x, texture2D(UTexture, vTextureCoord).x, texture2D(VTexture, vTextureCoord).x, 1) * YUV2RGB;","}"].join("\n"),h=i.createShader(i.FRAGMENT_SHADER);i.shaderSource(h,s),i.compileShader(h),i.attachShader(a,n),i.attachShader(a,h),i.linkProgram(a),i.useProgram(a),i.getProgramParameter(a,i.LINK_STATUS)||console.log("Shader link failed.");var u=i.getAttribLocation(a,"aVertexPosition");i.enableVertexAttribArray(u);var c=i.getAttribLocation(a,"aTextureCoord");i.enableVertexAttribArray(c);var f=i.createBuffer();i.bindBuffer(i.ARRAY_BUFFER,f),i.bufferData(i.ARRAY_BUFFER,new Float32Array([1,1,0,-1,1,0,1,-1,0,-1,-1,0]),i.STATIC_DRAW),i.vertexAttribPointer(u,3,i.FLOAT,!1,0,0);var v=i.createBuffer();return i.bindBuffer(i.ARRAY_BUFFER,v),i.bufferData(i.ARRAY_BUFFER,new Float32Array([1,0,0,0,1,1,0,1]),i.STATIC_DRAW),i.vertexAttribPointer(c,2,i.FLOAT,!1,0,0),i.y=new t(i),i.u=new t(i),i.v=new t(i),i.y.bind(0,a,"YTexture"),i.u.bind(1,a,"UTexture"),i.v.bind(2,a,"VTexture"),i}}}},t={};function r(i){var a=t[i];if(void 0!==a)return a.exports;var o=t[i]={exports:{}};return e[i](o,o.exports,r),o.exports}r.n=e=>{var t=e&&e.__esModule?()=>e.default:()=>e;return r.d(t,{a:t}),t},r.d=(e,t)=>{for(var i in t)r.o(t,i)&&!r.o(e,i)&&Object.defineProperty(e,i,{enumerable:!0,get:t[i]})},r.o=(e,t)=>Object.prototype.hasOwnProperty.call(e,t),(()=>{"use strict";var e=r(465),t=r.n(e);const i=function(){function e(e){e=e||{},this.canvasElement=e.canvas||document.createElement("canvas"),this.contextOptions=e.contextOptions,this.type=e.type||"yuv420",this.customYUV444=e.customYUV444,this.conversionType=e.conversionType||"rec601",this.width=e.width||640,this.height=e.height||320,this.animationTime=e.animationTime||0,this.canvasElement.width=this.width,this.canvasElement.height=this.height,this.initContextGL(),this.contextGL&&(this.initProgram(),this.initBuffers(),this.initTextures()),"yuv420"===this.type?this.drawNextOuptutPictureGL=function(e){var t=this.contextGL,r=this.texturePosBuffer,i=this.uTexturePosBuffer,a=this.vTexturePosBuffer,o=this.yTextureRef,n=this.uTextureRef,s=this.vTextureRef,h=e.yData,u=e.uData,c=e.vData,f=this.width,v=this.height,x=e.yDataPerRow||f,d=e.yRowCnt||v,T=e.uDataPerRow||f/2,g=e.uRowCnt||v/2,l=e.vDataPerRow||T,R=e.vRowCnt||g;t.viewport(0,0,f,v);var m=v/d,E=f/x,A=new Float32Array([E,0,0,0,E,m,0,m]);t.bindBuffer(t.ARRAY_BUFFER,r),t.bufferData(t.ARRAY_BUFFER,A,t.DYNAMIC_DRAW),this.customYUV444?(m=v/g,E=f/T):(m=v/2/g,E=f/2/T);var p=new Float32Array([E,0,0,0,E,m,0,m]);t.bindBuffer(t.ARRAY_BUFFER,i),t.bufferData(t.ARRAY_BUFFER,p,t.DYNAMIC_DRAW),this.customYUV444?(m=v/R,E=f/l):(m=v/2/R,E=f/2/l);var _=new Float32Array([E,0,0,0,E,m,0,m]);t.bindBuffer(t.ARRAY_BUFFER,a),t.bufferData(t.ARRAY_BUFFER,_,t.DYNAMIC_DRAW),t.activeTexture(t.TEXTURE0),t.bindTexture(t.TEXTURE_2D,o),t.texImage2D(t.TEXTURE_2D,0,t.LUMINANCE,x,d,0,t.LUMINANCE,t.UNSIGNED_BYTE,h),t.activeTexture(t.TEXTURE1),t.bindTexture(t.TEXTURE_2D,n),t.texImage2D(t.TEXTURE_2D,0,t.LUMINANCE,T,g,0,t.LUMINANCE,t.UNSIGNED_BYTE,u),t.activeTexture(t.TEXTURE2),t.bindTexture(t.TEXTURE_2D,s),t.texImage2D(t.TEXTURE_2D,0,t.LUMINANCE,l,R,0,t.LUMINANCE,t.UNSIGNED_BYTE,c),t.drawArrays(t.TRIANGLE_STRIP,0,4)}:"yuv422"===this.type&&(this.drawNextOuptutPictureGL=function(e){var t=this.contextGL,r=this.texturePosBuffer,i=this.textureRef,a=e.data,o=this.width,n=this.height,s=e.dataPerRow||2*o,h=e.rowCnt||n;t.viewport(0,0,o,n);var u=n/h,c=o/(s/2),f=new Float32Array([c,0,0,0,c,u,0,u]);t.bindBuffer(t.ARRAY_BUFFER,r),t.bufferData(t.ARRAY_BUFFER,f,t.DYNAMIC_DRAW),t.uniform2f(t.getUniformLocation(this.shaderProgram,"resolution"),s,n),t.activeTexture(t.TEXTURE0),t.bindTexture(t.TEXTURE_2D,i),t.texImage2D(t.TEXTURE_2D,0,t.LUMINANCE,s,h,0,t.LUMINANCE,t.UNSIGNED_BYTE,a),t.drawArrays(t.TRIANGLE_STRIP,0,4)})}return e.prototype.isWebGL=function(){return this.contextGL},e.prototype.initContextGL=function(){for(var e=this.canvasElement,t=null,r=["webgl","experimental-webgl","moz-webgl","webkit-3d"],i=0;!t&&i<r.length;){var a=r[i];try{t=this.contextOptions?e.getContext(a,this.contextOptions):e.getContext(a)}catch(e){t=null}t&&"function"==typeof t.getParameter||(t=null),++i}this.contextGL=t},e.prototype.initProgram=function(){var e,t,r=this.contextGL;"yuv420"===this.type?(e=["attribute vec4 vertexPos;","attribute vec4 texturePos;","attribute vec4 uTexturePos;","attribute vec4 vTexturePos;","varying vec2 textureCoord;","varying vec2 uTextureCoord;","varying vec2 vTextureCoord;","void main()","{","  gl_Position = vertexPos;","  textureCoord = texturePos.xy;","  uTextureCoord = uTexturePos.xy;","  vTextureCoord = vTexturePos.xy;","}"].join("\n"),t=["precision highp float;","varying highp vec2 textureCoord;","varying highp vec2 uTextureCoord;","varying highp vec2 vTextureCoord;","uniform sampler2D ySampler;","uniform sampler2D uSampler;","uniform sampler2D vSampler;","uniform mat4 YUV2RGB;","void main(void) {","  highp float y = texture2D(ySampler,  textureCoord).r;","  highp float u = texture2D(uSampler,  uTextureCoord).r;","  highp float v = texture2D(vSampler,  vTextureCoord).r;","  gl_FragColor = vec4(y, u, v, 1) * YUV2RGB;","}"].join("\n")):"yuv422"===this.type&&(e=["attribute vec4 vertexPos;","attribute vec4 texturePos;","varying vec2 textureCoord;","void main()","{","  gl_Position = vertexPos;","  textureCoord = texturePos.xy;","}"].join("\n"),t=["precision highp float;","varying highp vec2 textureCoord;","uniform sampler2D sampler;","uniform highp vec2 resolution;","uniform mat4 YUV2RGB;","void main(void) {","  highp float texPixX = 1.0 / resolution.x;","  highp float logPixX = 2.0 / resolution.x;","  highp float logHalfPixX = 4.0 / resolution.x;","  highp float steps = floor(textureCoord.x / logPixX);","  highp float uvSteps = floor(textureCoord.x / logHalfPixX);","  highp float y = texture2D(sampler, vec2((logPixX * steps) + texPixX, textureCoord.y)).r;","  highp float u = texture2D(sampler, vec2((logHalfPixX * uvSteps), textureCoord.y)).r;","  highp float v = texture2D(sampler, vec2((logHalfPixX * uvSteps) + texPixX + texPixX, textureCoord.y)).r;","  gl_FragColor = vec4(y, u, v, 1.0) * YUV2RGB;","}"].join("\n"));var i;i="rec709"==this.conversionType?[1.16438,0,1.79274,-.97295,1.16438,-.21325,-.53291,.30148,1.16438,2.1124,0,-1.1334,0,0,0,1]:[1.16438,0,1.59603,-.87079,1.16438,-.39176,-.81297,.52959,1.16438,2.01723,0,-1.08139,0,0,0,1];var a=r.createShader(r.VERTEX_SHADER);r.shaderSource(a,e),r.compileShader(a),r.getShaderParameter(a,r.COMPILE_STATUS)||console.log("Vertex shader failed to compile: "+r.getShaderInfoLog(a));var o=r.createShader(r.FRAGMENT_SHADER);r.shaderSource(o,t),r.compileShader(o),r.getShaderParameter(o,r.COMPILE_STATUS)||console.log("Fragment shader failed to compile: "+r.getShaderInfoLog(o));var n=r.createProgram();r.attachShader(n,a),r.attachShader(n,o),r.linkProgram(n),r.getProgramParameter(n,r.LINK_STATUS)||console.log("Program failed to compile: "+r.getProgramInfoLog(n)),r.useProgram(n);var s=r.getUniformLocation(n,"YUV2RGB");r.uniformMatrix4fv(s,!1,i),this.shaderProgram=n},e.prototype.initBuffers=function(){var e=this.contextGL,t=this.shaderProgram,r=e.createBuffer();e.bindBuffer(e.ARRAY_BUFFER,r),e.bufferData(e.ARRAY_BUFFER,new Float32Array([1,1,-1,1,1,-1,-1,-1]),e.STATIC_DRAW);var i=e.getAttribLocation(t,"vertexPos");if(e.enableVertexAttribArray(i),e.vertexAttribPointer(i,2,e.FLOAT,!1,0,0),this.animationTime){var a=this.animationTime,o=0,n=function(){var r=1*(o+=15)/a;o>=a?r=1:setTimeout(n,15);var i=-1*r,s=1*r,h=e.createBuffer();e.bindBuffer(e.ARRAY_BUFFER,h),e.bufferData(e.ARRAY_BUFFER,new Float32Array([s,s,i,s,s,i,i,i]),e.STATIC_DRAW);var u=e.getAttribLocation(t,"vertexPos");e.enableVertexAttribArray(u),e.vertexAttribPointer(u,2,e.FLOAT,!1,0,0);try{e.drawArrays(e.TRIANGLE_STRIP,0,4)}catch(e){}};n()}var s=e.createBuffer();e.bindBuffer(e.ARRAY_BUFFER,s),e.bufferData(e.ARRAY_BUFFER,new Float32Array([1,0,0,0,1,1,0,1]),e.STATIC_DRAW);var h=e.getAttribLocation(t,"texturePos");if(e.enableVertexAttribArray(h),e.vertexAttribPointer(h,2,e.FLOAT,!1,0,0),this.texturePosBuffer=s,"yuv420"===this.type){var u=e.createBuffer();e.bindBuffer(e.ARRAY_BUFFER,u),e.bufferData(e.ARRAY_BUFFER,new Float32Array([1,0,0,0,1,1,0,1]),e.STATIC_DRAW);var c=e.getAttribLocation(t,"uTexturePos");e.enableVertexAttribArray(c),e.vertexAttribPointer(c,2,e.FLOAT,!1,0,0),this.uTexturePosBuffer=u;var f=e.createBuffer();e.bindBuffer(e.ARRAY_BUFFER,f),e.bufferData(e.ARRAY_BUFFER,new Float32Array([1,0,0,0,1,1,0,1]),e.STATIC_DRAW);var v=e.getAttribLocation(t,"vTexturePos");e.enableVertexAttribArray(v),e.vertexAttribPointer(v,2,e.FLOAT,!1,0,0),this.vTexturePosBuffer=f}},e.prototype.initTextures=function(){var e=this.contextGL,t=this.shaderProgram;if("yuv420"===this.type){var r=this.initTexture(),i=e.getUniformLocation(t,"ySampler");e.uniform1i(i,0),this.yTextureRef=r;var a=this.initTexture(),o=e.getUniformLocation(t,"uSampler");e.uniform1i(o,1),this.uTextureRef=a;var n=this.initTexture(),s=e.getUniformLocation(t,"vSampler");e.uniform1i(s,2),this.vTextureRef=n}else if("yuv422"===this.type){var h=this.initTexture(),u=e.getUniformLocation(t,"sampler");e.uniform1i(u,0),this.textureRef=h}},e.prototype.initTexture=function(){var e=this.contextGL,t=e.createTexture();return e.bindTexture(e.TEXTURE_2D,t),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_MAG_FILTER,e.NEAREST),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_MIN_FILTER,e.NEAREST),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_WRAP_S,e.CLAMP_TO_EDGE),e.texParameteri(e.TEXTURE_2D,e.TEXTURE_WRAP_T,e.CLAMP_TO_EDGE),e.bindTexture(e.TEXTURE_2D,null),t},e.prototype.drawNextOutputPicture=function(e,t,r,i){this.contextGL?this.drawNextOuptutPictureGL(e,t,r,i):this.drawNextOuptutPictureRGBA(e,t,r,i)},e.prototype.drawNextOuptutPictureRGBA=function(e,t,r,i){var a=i,o=this.canvasElement.getContext("2d"),n=o.getImageData(0,0,e,t);n.data.set(a),o.putImageData(n,0,0)},e}();var a=function(e){var t=this;this._config=e||{},this.render=!0,!1===this._config.render&&(this.render=!1),this._config.useWorker=!0,this._config.workerFile=this._config.workerFile||"/js/broadway.js",this._config.preserveDrawingBuffer&&(this._config.contextOptions=this._config.contextOptions||{},this._config.contextOptions.preserveDrawingBuffer=!0),this.webgl=!0,this.webgl?(this.createCanvasObj=this.createCanvasWebGL,this.renderFrame=this.renderFrameWebGL):(this.createCanvasObj=this.createCanvasRGB,this.renderFrame=this.renderFrameRGB);var r=function(e,r,i,a){t.onPictureDecoded(e,r,i,a),e&&t.render&&(t.renderFrame({canvasObj:t.canvasObj,data:e,width:r,height:i}),t.onRenderFrameComplete&&t.onRenderFrameComplete({data:e,width:r,height:i,infos:a,canvasObj:t.canvasObj}))};if(this._config.size||(this._config.size={}),this._config.size.width=this._config.size.width||200,this._config.size.height=this._config.size.height||200,!this._config.useWorker)throw"Use worker only";var i=new Worker(this._config.workerFile);this.worker=i,i.addEventListener("message",(function(e){var i=e.data;i.consoleLog?console.log(i.consoleLog):r.call(t,new Uint8Array(i.buf,0,i.length),i.width,i.height,i.infos)}),!1),i.postMessage({type:"Broadway.js - Worker init",options:{rgb:!this.webgl,memsize:this.memsize,reuseMemory:!!this._config.reuseMemory}}),this._config.transferMemory?this.decode=function(e,t){i.postMessage({buf:e.buffer,offset:e.byteOffset,length:e.length,info:t},[e.buffer])}:this.decode=function(e,t){var r=new Uint8Array(e.length);r.set(e,0,e.length),i.postMessage({buf:r.buffer,offset:0,length:e.length,info:t},[r.buffer])},this._config.reuseMemory&&(this.recycleMemory=function(e){i.postMessage({reuse:e.buffer},[e.buffer])}),this.render&&(this.canvasObj=this.createCanvasObj(this._config),this.canvas=this.canvasObj.canvas),this.domNode=this.canvas,this.startWs()};a.prototype={onPictureDecoded:function(e,t,r,i){},startWs:function(){this.ws=new WebSocket(this._config.url),this.ws.addEventListener("message",this.onWsMessage.bind(this)),this.ws.addEventListener("close",this.onWsClose.bind(this))},onWsMessage:function(e){e.data.arrayBuffer().then((e=>{const t=new Int8Array(e);this.decode(t)}))},onWsClose:function(){setTimeout((()=>{this.destroyed||this.startWs()}),2e3)},destroy:function(){this.destroyed=!0,this.ws.removeEventListener("message",this.onWsMessage),this.ws.removeEventListener("close",this.onWsClose),this.ws.close(),this.worker?.terminate()},recycleMemory:function(e){},createCanvasWebGL:function(e){var t=this._createBasicCanvasObj(e);return t.contextOptions=e.contextOptions,t},createCanvasRGB:function(e){return this._createBasicCanvasObj(e)},_createBasicCanvasObj:function(e){var t={},r=(e=e||{}).width;r||(r=this._config.size.width);var i=e.height;return i||(i=this._config.size.height),t.canvas=e.canvas??document.createElement("canvas"),t.canvas.width=r,t.canvas.height=i,t.canvas.style.backgroundColor="#0D0E1B",t},renderFrameWebGL:function(e){var t=e.canvasObj,r=e.width||t.canvas.width,a=e.height||t.canvas.height;t.canvas.width===r&&t.canvas.height===a&&t.webGLCanvas||(t.canvas.width=r,t.canvas.height=a,t.webGLCanvas=new i({canvas:t.canvas,contextOptions:t.contextOptions,width:r,height:a}));var o=r*a,n=r/2*(a/2);t.webGLCanvas.drawNextOutputPicture({yData:e.data.subarray(0,o),uData:e.data.subarray(o,o+n),vData:e.data.subarray(o+n,o+n+n)}),this.recycleMemory(e.data)},renderFrameRGB:function(e){var t=e.canvasObj,r=e.width||t.canvas.width,i=e.height||t.canvas.height;t.canvas.width===r&&t.canvas.height===i||(t.canvas.width=r,t.canvas.height=i);var a=t.ctx,o=t.imgData;a||(t.ctx=t.canvas.getContext("2d"),a=t.ctx,t.imgData=a.createImageData(r,i),o=t.imgData),o.data.set(e.data),a.putImageData(o,0,0),this.recycleMemory(e.data)}};const o=a;window.WebDecoder=class{constructor(e){const{url:r,canvas:i,type:a,width:n,height:s}=e;if("h264"===a)this.decoder=new o(e);else{if("h265"!==a)throw"Unknown stream type";this.decoder=new class{constructor(e){const{canvas:r,url:i}=e;this.worker=new Worker("js/missile.js"),this.url=i,this.canvas=r,this.yuv=t().setupCanvas(this.canvas,{preserveDrawingBuffer:!1}),this.worker.onmessage=e=>{t().renderFrame(this.yuv,...e.data)},this.start()}start(){this.ws&&this.stop(),this.ws=new WebSocket(this.url),this.ws.addEventListener("message",(e=>{e.data.arrayBuffer().then((e=>{const t=new Int8Array(e);this.worker.postMessage(t)}))}))}stop(){this.ws.close(),this.ws=null}}(e)}this.url=r,this.type=a,this.canvas=i}}})()})();