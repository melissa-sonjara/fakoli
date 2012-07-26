/*!
 * Raphael Emboss Plugin 0.1
 *
 * by Andy Green
 *
 * Based on Raphael Dropshadow by Daniel Hoffman
 * Based on Raphael Blur Plugin 0.1 by Dmitry Baranovskiy (http://raphaeljs.com)
 * https://github.com/DmitryBaranovskiy/raphael/blob/master/plugins/raphael.blur.js
 * Licensed under the MIT (http://www.opensource.org/licenses/mit-license.php) license.
 *
 */

(function() {
	if(Raphael.vml) {
		Raphael.el.emboss = function (size, offsetX, offsetY) {
            // not supporting VML yet
            return this; // maintain chainability
        }
	} else {
		var $ = function(el, attr) {
			if(attr) {
				for(var key in attr)
				if(attr.hasOwnProperty(key)) {
					el.setAttribute(key, attr[key]);
				}
			} else {
				return document.createElementNS("http://www.w3.org/2000/svg", el);
			}
		};
		
	/*	  <filter id="emboss" >
		    <feGaussianBlur in="SourceAlpha" stdDeviation="2" result="blur"/>
		    <feSpecularLighting in="blur" surfaceScale="-3" style="lighting-color:white"
			   specularConstant="1" specularExponent="16" result="spec" kernelUnitLength="1" >
		 	<feDistantLight azimuth="45" elevation="45" />
		    </feSpecularLighting>
		    <feComposite in="spec" in2="SourceGraphic" operator="in" result="specOut"/>
		  </filter> */

		  
		Raphael.el.emboss = function() 
		{
			var fltr = $("filter"), 
				blur = $("feGaussianBlur"), 
				specular = $("feSpecularLighting"),
				distant = $("feDistantLight"),
				composite = $("feComposite"),
				composite2 = $("feComposite");
				
			fltr.id = "emboss";

			$(blur, {"stdDeviation" : 2, "in": "SourceAlpha", "result": "blur"});

			$(specular, {"in": "blur", 
						 "surfaceScale": -3, 
						 "style": "lighting-color: white",
						 "specularConstant": 1,
						 "specularExponent": 40,
						 "result": "spec",
						 "kernelUnitLength": 1});
			
			$(distant, {"azimuth": 45, "elevation": 45});
			
			$(composite, {"in": "spec", "in2": "SourceGraphic", "operator": "in", "result": "specOut"});
			$(composite2, {"in": "SourceGraphic",  "in2": "specOut", "operator": "arithmetic", 
                		   "k1": "0", "k2": "1", "k3": "1", "k4": "0", "result": "litPaint"});
			fltr.appendChild(blur);
			specular.appendChild(distant);
			fltr.appendChild(specular);
			fltr.appendChild(composite);
			fltr.appendChild(composite2);
			this.paper.defs.appendChild(fltr);
			this._emboss = fltr;
			$(this.node, {
				"filter" : "url(#" + fltr.id + ")"
			});

			return this;  // maintain chainability
		};
	}
	Raphael.st.emboss = function(size, offsetX, offsetY) {
			return this.forEach(function(el) {
				el.emboss(size, offsetX, offsetY);
			});
		};
})();