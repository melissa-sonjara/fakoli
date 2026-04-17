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

String.prototype.count = function(s1)
{
	return (this.length - this.replace(new RegExp(s1, "g"), '').length) / s1.length;
};

if (!String.format)
{
	String.format = function(format)
	{
		var args = Array.prototype.slice.call(arguments, 1);
		return format.replace(/{(\d+)}/g, function(match, number)
		{
			return typeof args[number] != 'undefined' ? args[number] : match;
		});
	};
}

function popup(url, name, width, height)
{
	var styles;
	if (arguments.length == 5)
	{
		styles = arguments[4];
	}
	else
	{
		styles = "toolbar=0,location=1,scrollbars=1,resizable=1";
	}
	styles = 'width=' + width + ',height=' + height + ',' + styles;
	var w = window.open(url, name, styles);
	w.focus();
}

function go(url)
{
	location.href = url;
}

function httpRequest(url)
{
	var request = new XMLHttpRequest();
	request.open("GET", url, false);
	request.send("");
	if (request.status == 200)
	{
		return request.responseText;
	}
	else
	{
		return "<span style='color:red;font-weight:bold'>Error retrieving data</span>";
	}
}

function todayLong()
{
	var monthNames = ["January","February","March","April","May","June","July",
		"August","September","October","November","December"];
	var now = new Date();
	return now.getDate() + " " + monthNames[now.getMonth()] + " " + now.getFullYear();
}

function basename(str)
{
	var idx = str.lastIndexOf('\\');
	if (idx == -1) idx = str.lastIndexOf('/');
	if (idx == -1) return str;
	return str.substring(idx + 1);
}

function dirname(str)
{
	var idx = str.lastIndexOf('\\');
	if (idx == -1) idx = str.lastIndexOf('/');
	if (idx == -1) return str;
	return str.substring(0, idx);
}

function maskInput(e, allowNeg)
{
	var key;
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
	var keychar = String.fromCharCode(key);
	if ((key == null) || (key == 0) || (key == 8) || (key == 9) || (key == 13) || (key == 27))
	{
		return true;
	}
	else if (numbers.indexOf(keychar) > -1)
	{
		return true;
	}
	return false;
}

function isArray(obj)
{
	return Array.isArray(obj);
}

function formatCurrency(val)
{
	var nStr = val.toFixed(2) + '';
	var x = nStr.split('.');
	var x1 = x[0];
	var x2 = x.length > 1 ? '.' + x[1] : '';
	var rgx = /(\d+)(\d{3})/;
	while (rgx.test(x1))
	{
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
	if (temp.length == 0) return false;
	for (var i = temp.length - 1; i >= 0; --i)
	{
		incNum = parseInt(temp.charAt(i), 10);
		counter += (odd = !odd) ? incNum : luhnArr[incNum];
	}
	return (counter % 10 == 0);
}

function isDefined(obj)
{
	return typeof(obj) != "undefined";
}

function findAncestor(element, tag)
{
	if (typeof element === 'string') element = document.getElementById(element);
	tag = tag.toUpperCase();
	do
	{
		if (element.tagName && element.tagName.toUpperCase() == tag) return element;
		element = element.parentElement;
	} while (element);
	return null;
}

function isHidden(element)
{
	if (typeof element === 'string') element = document.getElementById(element);
	while (element)
	{
		var style = window.getComputedStyle(element);
		if (style.display == 'none') return true;
		if (style.visibility == 'hidden') return true;
		element = element.parentElement;
	}
	return false;
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
	name = name.replace(/^(.)|\s+(.)/g, function($1)
	{
		return $1.toUpperCase();
	});
	return name;
}

function number_format(number, decimals, dec_point, thousands_sep)
{
	// Formats a number with grouped thousands
	number = (number + '').replace(',', '').replace(' ', '');
	var n = !isFinite(+number) ? 0 : +number,
		prec = !isFinite(+decimals) ? 0 : Math.abs(decimals),
		sep = (typeof thousands_sep === 'undefined') ? ',' : thousands_sep,
		dec = (typeof dec_point === 'undefined') ? '.' : dec_point,
		s = '',
		toFixedFix = function(n, prec)
		{
			var k = Math.pow(10, prec);
			return '' + Math.round(n * k) / k;
		};
	s = (prec ? toFixedFix(n, prec) : '' + Math.round(n)).split('.');
	if (s[0].length > 3)
	{
		s[0] = s[0].replace(/\B(?=(?:\d{3})+(?!\d))/g, sep);
	}
	if ((s[1] || '').length < prec)
	{
		s[1] = s[1] || '';
		s[1] += new Array(prec - s[1].length + 1).join('0');
	}
	return s.join(dec);
}

function u_atob(ascii)
{
	return Uint8Array.from(atob(ascii), c => c.charCodeAt(0));
}

function u_btoa(buffer)
{
	var binary = [];
	var bytes = new Uint8Array(buffer);
	for (var i = 0, il = bytes.byteLength; i < il; i++)
	{
		binary.push(String.fromCharCode(bytes[i]));
	}
	return btoa(binary.join(''));
}

function htmlencode(rawStr)
{
	return rawStr.replace(/[\u00A0-\u9999<>\&]/g, function(i)
	{
		return '&#' + i.charCodeAt(0) + ';';
	});
}
