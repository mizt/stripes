(function() {

	window.indicator = window.indicator || {}

	// interface
	var settings = {
		color:"#000",
		weight:3,
		radius:80
	} 

	Object.keys(settings).forEach(function (key) {
		if(window.indicator[key]) {
			settings[key] = window.indicator[key]
			delete window.indicator[key];
		}
	});
	
	window.indicator.instance = window.indicator.instance || new (function() {
		
		return new ((function() {

			var _instance=function(){};

		// private:
			
			var _timerID = 0;
			
			var _container = undefined;
			var _indicator = undefined;
			
			var _style = {
				color  : "#000",
				weight  : 3,
				radius : 100
			};
			
			var _exec = function() {
				_indicator.style.border = _style["weight"]+"px solid "+_style["color"];
				_indicator.style.width = _indicator.style.height = _style["radius"]+"px";
				_indicator.style.top = _indicator.style.left = "-"+(_style["weight"]+(_style["radius"]>>1))+"px";				
				_indicator.style.borderRightColor = "transparent"; 
			};
			
		// public:

			var _public = _instance.prototype;
			
			_public.start = function() {						
				if(_timerID) {
					clearInterval(_timerID);
					_timerID = 0;
					if(document.getElementById("indicatorContainer")) {
						document.body.removeChild(_container);
					}
				}
				
				if(window.addEventListener&&document.getElementById("indicatorContainer")==undefined) {
					_container = document.createElement("div");
					_container.id = "indicatorContainer";					
					_indicator = document.createElement("div");
					_indicator.id = "indicator";
					_container.appendChild(_indicator);               
					document.body.appendChild(_container);
					_exec();
				}
			}
			
			_public.stop = function() {	
				if(_indicator) {
					_container.style.opacity = 0.0;
					_timerID = setTimeout(function() {
						if(document.getElementById("indicatorContainer")) {
							document.body.removeChild(_container);
						}			
						_timerID = 0;
					},2000);
				}				
			}
			
		// constructor
		
			var initalize = function(args) { 
				
				if(args) {
					if(args.color !==undefined) _style["color"]  = args.color;
					if(args.weight!==undefined) _style["weight"] = args.weight;
					if(args.radius!==undefined) _style["radius"] = args.radius;
				}
				
				this.start();
				return _instance;
			}
				
			return initalize.bind(_public)(arguments[0]);
			
		})(arguments[0]));
		
	})(settings);
	
	window.indicator.stop = window.indicator.instance.stop;

})();