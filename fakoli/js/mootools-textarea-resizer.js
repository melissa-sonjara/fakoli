/**
description: Mootools TextareaResizer plugin for Mootools 1.2.x

license: MIT-style license.

copyright: Copyright (c) 2009 Joshua Partogi (http://scrum8.com/).

authors: Joshua Partogi (http://scrum8.com/)

The MIT License

Copyright (c) 2009 Joshua Partogi (http://scrum8.com/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
var TextareaResizer = new Class({

    initialize: function(element){
        this.textarea = element;
        this.element = element;
    },

    resizable: function(){

    	
        var staticOffset = 0;
        var iLastMousePos = 0;
        var iMin = 32;

        var textarea = this.element;
       	if (textarea.hasClass("fixed-size")) return;
       	
        textarea.addClass('processed');
        
        var div = new Element('div').addClass('resizable-textarea');
        var span = new Element('span');

        div.wraps(span.wraps(textarea));

        var grippie = new Element('div').addClass('grippie');
        grippie.inject(span);
        grippie.setStyle('margin-right', grippie.offsetWidth - textarea.offsetWidth);
        grippie.setStyle('width', textarea.getStyle('width'));
        grippie.setStyles({'padding': 0, 'margin': 0});

        var endDrag = function(e) {            
            document.removeEvent('mousemove', performDrag);
            document.removeEvent('mouseup', endDrag);
            
            textarea.setStyle('opacity', 1.0);
            textarea.focus();

            staticOffset = 0;
            iLastMousePos = 0;
        };

        var performDrag = function(e) {
            var iThisMousePos = mousePosition(e).y;
            var iMousePos = staticOffset + iThisMousePos;

            if (iLastMousePos >= (iThisMousePos)) {
                iMousePos -= 5;
            }

            iLastMousePos = iThisMousePos;
            iMousePos = Math.max(iMin, iMousePos);

            textarea.setStyle('height', iMousePos);

            if (iMousePos < iMin) {
                endDrag(e);
            }
        };

        var startDrag = function(e){
            textarea.blur();

            iLastMousePos = mousePosition(e).y;
            staticOffset = textarea.getSize().y - iLastMousePos;
            textarea.setStyle('opacity', 0.25);
            
            document.addEvent('mousemove', performDrag);
            document.addEvent('mouseup', endDrag);
        };

        var mousePosition = function(e) {
            return {
                x: e.event.clientX + document.documentElement.scrollLeft,
                y: e.event.clientY + document.documentElement.scrollTop
            };
        };

        grippie.addEvent('mousedown', startDrag);
    }
    

});


(function(){

    Element.implement({

        resizable: function(){
            var resizer = new TextareaResizer(this);

            resizer.resizable();
        }
        
    });

})();


// AJG - Automatically apply to all textareas on the page on load
window.addEvent('domready', function() 
{
  	$$('textarea').resizable();
});  