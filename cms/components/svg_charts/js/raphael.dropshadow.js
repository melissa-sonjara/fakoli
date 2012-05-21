/*!
 * Raphael Drop Shadow Plugin 0.1
 *
 * by Daniel Hoffmann
 *
 * Based on Raphael Blur Plugin 0.1 by Dmitry Baranovskiy (http://raphaeljs.com)
 * https://github.com/DmitryBaranovskiy/raphael/blob/master/plugins/raphael.blur.js
 * Licensed under the MIT (http://www.opensource.org/licenses/mit-license.php) license.
 *
 */

(function() {
	if(Raphael.vml) {
		Raphael.el.dropShadow = function (size, offsetX, offsetY) {
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
		Raphael.el.dropShadow = function(size, offsetX, offsetY, opacity) {
			if(size != "none") {
				var fltr = $("filter"), 
					blur = $("feGaussianBlur"), 
					offset = $("feOffset"), 
					transfer = $("feComponentTransfer"),
					transferFn = $("feFuncA"),
					merge = $("feMerge"), 
					mergeNodeShadow = $("feMergeNode"), 
					mergeNodeSource = $("feMergeNode");
				id = ("dropshadow_" + size + "_" + offsetX + "_" + offsetY + "_" + opacity).replace(".", "_");
				fltr.id = id;
				$(fltr, {
					"height" : "150%",
					"width" : "150%"
				});
				$(blur, {"stdDeviation" : +size});
				$(blur, {"in": "SourceAlpha"});
				$(offset, {
					"dx" : offsetX,
					"dy" : offsetY,
					"result" : "offsetblur"
				});
				$(mergeNodeShadow, {"in": "offsetblur"});
				$(mergeNodeSource, {"in": "SourceGraphic"});
				$(transferFn, {"type": "linear", "slope": opacity});
				transfer.appendChild(transferFn);
				fltr.appendChild(blur);
				fltr.appendChild(offset);
				fltr.appendChild(transfer);
				fltr.appendChild(merge);
				merge.appendChild(mergeNodeShadow);
				merge.appendChild(mergeNodeSource);
				this.paper.defs.appendChild(fltr);
				this._blur = fltr;
				$(this.node, {
					"filter" : "url(#" + id + ")"
				});
			} else {
				if(this._blur) {
					this._blur.parentNode.removeChild(this._blur);
					delete this._blur;
				}
				this.node.removeAttribute("filter");
			}
			return this;  // maintain chainability
		};
	}
	Raphael.st.dropShadow = function(size, offsetX, offsetY) {
			return this.forEach(function(el) {
				el.dropShadow(size, offsetX, offsetY);
			});
		};
})();