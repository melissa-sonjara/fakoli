/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/


String.prototype.count=function(s1) { 
    return (this.length - this.replace(new RegExp(s1,"g"), '').length) / s1.length;
}

if (!String.format) 
{
  String.format = function(format) {
    var args = Array.prototype.slice.call(arguments, 1);
    return format.replace(/{(\d+)}/g, function(match, number) { 
      return typeof args[number] != 'undefined'
        ? args[number] 
        : match
      ;
    });
  };
}

function popup(url, name, width, height)
{
	if (arguments.length == 5)
	{
		styles = arguments[4];
	}
	else
	{
		styles = "toolbar=0,location=1,scrollbars=1,resizable=1";
	}
	
	styles = 'width='+width+',height='+height+','+styles;
	var w = window.open(url, name, styles);
	w.focus();
}

function go(url)
{
	location.href = url;
}


function httpRequest(url)
{
	var ua = navigator.userAgent.toLowerCase();
   	if (!window.ActiveXObject)
   	{
    	request = new XMLHttpRequest();
   	}
   	else if (ua.indexOf('msie 5') == -1)
   	{
     	request = new ActiveXObject("Msxml2.XMLHTTP");
   	}
   	else
   	{
   		request = new ActiveXObject("Microsoft.XMLHTTP");
   	}

	// the request variable holds an XMLHttpRequest 
	// object instance
	request.open("GET", url, false);
	request.send("");

	var result = "";

	if ( request.status == 200 )
	{
		result = request.responseText;
	}
	else
	{
		result = "<span style='color:red;font-weight:bold'>Error retrieving data</span>";
	}
	
	return result;	
}

function todayLong()
{
	var monthNames = new Array(
	"January","February","March","April","May","June","July",
	"August","September","October","November","December");

	var now = new Date();

	var today = now.getDate() + " " + monthNames[now.getMonth()] + " " + now.getFullYear();
	
	return today;
}

function basename(str)
{
	var idx = str.lastIndexOf('\\');
	if (idx == -1)
	{
		idx = str.lastIndexOf('/');
	}
	
	if (idx == -1) return str;
	
	return str.substring(idx + 1);
}

function dirname(str)
{
	var idx = str.lastIndexOf('\\');
	if (idx == -1)
	{
		idx = str.lastIndexOf('/');
	}
	
	if (idx == -1) return str;
	
	return str.substring(0, idx);
}

function maskInput(e, allowNeg)
{
	var key;
	var keychar;

	if (window.event)
	{
		key = window.event.keyCode;
	}
	else if (e)
	{
		key = e.which;
	 }
	else
	{
		return true;
	}
	
	if (typeof(allowNeg) == 'undefined') allowNeg = true;
	var numbers = allowNeg ? "0123456789.,-" : "0123456789.,";
	
	keychar = String.fromCharCode(key);
	
	// control keys
	if ((key==null) || (key==0) || (key==8) ||
		(key==9) || (key==13) || (key==27) )
	{
		return true;
	}
	// numbers
	else if ((numbers.indexOf(keychar) > -1))
	{
		return true;
	}
	
	return false;
}

function isArray(obj) {
   if (obj.constructor.toString().indexOf("Array") == -1)
      return false;
   else
      return true;
}

function formatCurrency(val)
{
	var nStr = val.toFixed(2);
	nStr += '';
	x = nStr.split('.');
	x1 = x[0];
	x2 = x.length > 1 ? '.' + x[1] : '';
	var rgx = /(\d+)(\d{3})/;
	while (rgx.test(x1)) {
		x1 = x1.replace(rgx, '$1' + ',' + '$2');
	}
	return "$" + x1 + x2;
}

function rawNumber(val)
{
	return Number(val.replace(/[^\d\.\-]/g, ''));
}

function luhnCheck(str)
{
	var luhnArr = [0, 2, 4, 6, 8, 1, 3, 5, 7, 9];
	var counter = 0;
	var incNum;
	var odd = false;
	var temp = String(str).replace(/[^\d]/g, "");
	if ( temp.length == 0)
		return false;
	for (var i = temp.length-1; i >= 0; --i)
	{
		incNum = parseInt(temp.charAt(i), 10);
		counter += (odd = !odd)? incNum : luhnArr[incNum];
	}
	return (counter%10 == 0);
}

function isDefined(obj)
{
	 if(typeof(obj) !="undefined") return true; else return false; 
}

function findAncestor(element, tag)
{
	element = document.id(element);
	tag = tag.toUpperCase();
	do
	{
		if (element.tagName.toUpperCase() == tag) return element;
		element = element.getParent();
	}
	while(element);
	
	return null;		
}

function isHidden(element)
{
	element = document.id(element);
	while(element)
	{
		if (element.getStyle('display') == 'none') return true;
		if (element.getStyle('visible') == 'hidden') return true;
		element = element.getParent();
	}
	
	return false;
}

function calculateZIndex(element)
{
	var zIndex = element.getComputedStyle('z-index');
	if (zIndex != 'auto' && zIndex != 0)
	{
		return zIndex;
	}
	
	var parent = element.getParent();
	if (!parent) return 0;
	return calculateZIndex(parent)
}

function appendQueryString(url, params)
{
	url += url.indexOf("?") >= 0 ? "&" : "?";
	url += params;
	
	return url;
}

function codify(name)
{
	name = name.replace(/[\s\W]+/, "_");
	name = name.replace("&", "and");
	return name;
}

function prettify(name)
{
	name = name.replace(/([a-z])([A-Z0-9])/, "$1 $2");
	name = name.replace("_", " ");
	name = name.replace(/^(.)|\s+(.)/g, function ($1) 
	{
      return $1.toUpperCase()
    });
	
	return $name;
}

function number_format (number, decimals, dec_point, thousands_sep) {
    // Formats a number with grouped thousands  
    // 
    // version: 1102.614
    // discuss at: http://phpjs.org/functions/number_format    // +   original by: Jonas Raoni Soares Silva (http://www.jsfromhell.com)
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +     bugfix by: Michael White (http://getsprink.com)
    // +     bugfix by: Benjamin Lupton
    // +     bugfix by: Allan Jensen (http://www.winternet.no)    // +    revised by: Jonas Raoni Soares Silva (http://www.jsfromhell.com)
    // +     bugfix by: Howard Yeend
    // +    revised by: Luke Smith (http://lucassmith.name)
    // +     bugfix by: Diogo Resende
    // +     bugfix by: Rival    // +      input by: Kheang Hok Chin (http://www.distantia.ca/)
    // +   improved by: davook
    // +   improved by: Brett Zamir (http://brett-zamir.me)
    // +      input by: Jay Klehr
    // +   improved by: Brett Zamir (http://brett-zamir.me)    // +      input by: Amir Habibi (http://www.residence-mixte.com/)
    // +     bugfix by: Brett Zamir (http://brett-zamir.me)
    // +   improved by: Theriault
    // +      input by: Amirouche
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)    // *     example 1: number_format(1234.56);
    // *     returns 1: '1,235'
    // *     example 2: number_format(1234.56, 2, ',', ' ');
    // *     returns 2: '1 234,56'
    // *     example 3: number_format(1234.5678, 2, '.', '');    // *     returns 3: '1234.57'
    // *     example 4: number_format(67, 2, ',', '.');
    // *     returns 4: '67,00'
    // *     example 5: number_format(1000);
    // *     returns 5: '1,000'    // *     example 6: number_format(67.311, 2);
    // *     returns 6: '67.31'
    // *     example 7: number_format(1000.55, 1);
    // *     returns 7: '1,000.6'
    // *     example 8: number_format(67000, 5, ',', '.');    // *     returns 8: '67.000,00000'
    // *     example 9: number_format(0.9, 0);
    // *     returns 9: '1'
    // *    example 10: number_format('1.20', 2);
    // *    returns 10: '1.20'    // *    example 11: number_format('1.20', 4);
    // *    returns 11: '1.2000'
    // *    example 12: number_format('1.2000', 3);
    // *    returns 12: '1.200'
    // *    example 13: number_format('1 000,50', 2, '.', ' ');    // *    returns 13: '100 050.00'
    number = (number + '').replace(',', '').replace(' ', '');
    var n = !isFinite(+number) ? 0 : +number,
        prec = !isFinite(+decimals) ? 0 : Math.abs(decimals),
        sep = (typeof thousands_sep === 'undefined') ? ',' : thousands_sep,        dec = (typeof dec_point === 'undefined') ? '.' : dec_point,
        s = '',
        toFixedFix = function (n, prec) {
            var k = Math.pow(10, prec);
            return '' + Math.round(n * k) / k;        };
    // Fix for IE parseFloat(0.55).toFixed(0) = 0;
    s = (prec ? toFixedFix(n, prec) : '' + Math.round(n)).split('.');
    if (s[0].length > 3) {
        s[0] = s[0].replace(/\B(?=(?:\d{3})+(?!\d))/g, sep);    }
    if ((s[1] || '').length < prec) {
        s[1] = s[1] || '';
        s[1] += new Array(prec - s[1].length + 1).join('0');
    }    return s.join(dec);
}




// Fix for Internet Explorer document.getElementById() bug.
// IE will falsely match meta tags when searching the DOM with the above method,

if (/msie/i.test (navigator.userAgent)) //only override IE
{	
	document.nativeGetElementById = document.getElementById;
	document.getElementById = function(id)
	{
		var elem = document.nativeGetElementById(id);
		if(elem)
		{
			//make sure that it is a valid match on id
			if(elem.attributes['id'].value == id)
			{
				return elem;
			}
			else
			{
				//otherwise find the correct element
				if (!document.all[id]) return null;
				
				if (document.all[id].length)
				{
					for(var i=1;i<document.all[id].length;i++)
					{
						if (document.all[id][i].attributes['id'] && 
							document.all[id][i].attributes['id'].value == id)
						{
							return document.all[id][i];
						}
					}
					
				}
				else
				{
					if (document.all[id].attributes['id'])
					{
						if (document.all[id].attributes['id'].value == id) return document.all[id];
					}
				}
			}
		}
		return null;
	};
}

function u_atob(ascii) {
    return Uint8Array.from(atob(ascii), c => c.charCodeAt(0));
}

function u_btoa(buffer) {
    var binary = [];
    var bytes = new Uint8Array(buffer);
    for (var i = 0, il = bytes.byteLength; i < il; i++) {
        binary.push(String.fromCharCode(bytes[i]));
    }
    return btoa(binary.join(''));
}

function htmlencode(rawStr)
{
	var encodedStr = rawStr.replace(/[\u00A0-\u9999<>\&]/g, function(i) {
		   return '&#'+i.charCodeAt(0)+';';
	});
	return encodedStr;
}

// Mock console object to prevent debugging calls from causing errors in IE.
// Thanks to http://skysanders.net/subtext/archive/2010/07/10/javascript-console-and-firebug-mocks.aspx
if ("undefined" === typeof window.console)
{
    window.console = {
        "log": function() { }
    };
};

if (!("undefined" !== typeof window.console && (window.console.firebugVersion || window.console.firebug )))
{
    window.console = {
        "assert": function() { },
        "count": function() { },
        "clear": function() { },
        "debug": function() { },
        "dir": function() { },
        "dirxml": function() { },
        "info": function() { },
        "error": function() { },
        "getFirebugElement": function() { },
        "group": function() { },
        "groupEnd": function() { },
        "groupCollapsed": function() { },
        "log": function() { },
        "notifyFirebug": function() { },
        "profile": function() { },
        "profileEnd": function() { },
        "time": function() { },
        "timeEnd": function() { },
        "trace": function() { },
        "warn": function() { },
        "userObjects": [],
        "element": {},
        "firebug": "foo"
    };
};

