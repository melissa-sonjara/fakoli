/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

//
// new SortingTable( 'my_table', {
// zebra: true, // Stripe the table, also on initialize
// details: false, // Has details every other row
// paginator: false, // Pass a paginator object
// onSorted: function(){}, // Callback to run after sort
// dont_sort_class: 'nosort', // Class name on th's that don't sort
// forward_sort_class: 'forward_sort', // Class applied to forward sort th's
// reverse_sort_class: 'reverse_sort' // Class applied to reverse sort th's
// });
//
// The above were the defaults. The regexes in load_conversions test a cell
// begin sorted for a match, then use that conversion for all elements on that
// column.
//
// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
 
var SortingTable = new Class(
{
 
	Implements: [Options, Events],
  
	options: 
	{
		zebra: true,
		details: false,
		paginator: false,
		filter: false,
		onSorted: function(){},
		dont_sort_class: 'nosort',
		forward_sort_class: 'forward_sort',
		reverse_sort_class: 'reverse_sort'
	},
 
	initialize: function( table, options ) 
	{
		this.table = document.id(table);
		this.setOptions(options);

		this.tbody = this.table.getElement('tbody');
		if (this.options.zebra) 
		{
			SortingTable.stripe_table(this.tbody.getChildren());
		}
 
	    this.headers = this.table.getElement('thead').getElements('th');
	    this.headers.each(function( header, index ) 
	    {
	    	if (header.hasClass( this.options.dont_sort_class )) { return; }
	    	header.store( 'column', index );
	    	header.addEvent( 'click', function(evt)
	    	{
	    		if (evt.target != header) return;
	    		this.sortByHeader( evt.target );
	    		if ( this.options.paginator) this.options.paginator.update_pages();
	    	}.bind( this ) );
	    }, this);
 
	    this.load_conversions();
	},
 
	sortByHeader: function( header )
	{
	    var rows = [];
	    
	    var before = this.tbody.getPrevious();
	    this.tbody.dispose();
	    
	    var trs = this.tbody.getChildren();
	    while ( row = trs.shift() ) 
	    {
		    row = { row: row.dispose() };
		    if ( this.options.details ) 
		    {
		    	row.detail = trs.shift().dispose();
		    }
		    rows.unshift( row );
	    }
    
	    if ( this.sort_column >= 0 &&
	         this.sort_column == header.retrieve('column') ) 
	    {
	    	// They were pulled off in reverse
	    	if ( header.hasClass( this.options.reverse_sort_class ) ) 
	    	{
	    		header.removeClass( this.options.reverse_sort_class );
	    		header.addClass( this.options.forward_sort_class );
	    		
	    	} 
	    	else 
	    	{
	    		header.removeClass( this.options.forward_sort_class );
	    		header.addClass( this.options.reverse_sort_class );
	    		
	    	}
	    } 
	    else 
	    {	    
	    	this.headers.each(function(h)
	    	{
	    		h.removeClass( this.options.forward_sort_class );
	    		h.removeClass( this.options.reverse_sort_class );
	    	}, this);
	    	
	    	this.sort_column = header.retrieve('column');
	    	
	    	if (header.get('data-sort'))
	    	{
	    		// Explict sort order already provided
	    		rows.each(function(row)
	    		{
	    			row.sort_column = this.sort_column;
	    			row.toString = function() { return this.row.getChildren('td')[this.sort_column].get('data-sort'); };
	    		}.bind(this));
		    	header.addClass( this.options.forward_sort_class );
	    	}
	    	else
	    	{
	    		if (header.retrieve('conversion_function')) 
	    		{
	    			this.conversion_matcher = header.retrieve('conversion_matcher');
	    			this.conversion_function = header.retrieve('conversion_function');
	    		} 
		    	else
		    	{
		    		this.conversion_function = false;
		    		rows.some(function(row)
		    		{
		    			var to_match = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
		    			if (to_match == '') return false;
		    			this.conversions.some(function(conversion)
		    			{
		    				if (conversion.matcher.test( to_match ))
		    				{
		    					this.conversion_matcher = conversion.matcher;
		    					this.conversion_function = conversion.conversion_function;
		    					return true;
		    				}
		    				return false;
		    			}, this);
		    			return !!(this.conversion_function);
		    		}, this);
		        
		    		header.store('conversion_function', this.conversion_function );
		        
		    		header.store('conversion_matcher', this.conversion_matcher );
		    	}
		    	header.addClass( this.options.forward_sort_class );
		    	rows.each(function(row)
		    	{
			        var compare_value = this.conversion_function( row );
			        row.toString = function()
			        {
			        	return compare_value;
			        };
		    	}, this);
	    	}

	    	rows.sort();
	    }
 
	    var index = 0;
	    while ( row = rows.shift() ) 
	    {
	    	this.tbody.appendChild(row.row);
	    	if (row.detail) this.tbody.appendChild(row.detail);
	    	if ( this.options.zebra && !row.row.hasClass('filtered')) 
	    	{
	    		row.row.removeClass('alt');
	    		if (row.detail) row.detail.removeClass('alt');
	    		
	    		if (index % 2) 
	    		{
	    			row.row.addClass( 'alt' );
	    			if (row.detail) row.detail.addClass( 'alt' );
	    		}
		    
	    		index++;
	    	}
	    }
	
	    this.tbody.inject(before, 'after');
	    this.fireEvent('sorted', this);
	},
 
	load_conversions: function() 
	{
		this.conversions = Array.convert([
		// 1.75 MB, 301 GB, 34 KB, 8 TB
		{ 
			matcher: /([0-9.]{1,8}).*([KMGT]{1})B/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				cell = this.conversion_matcher.exec( cell );
				if (!cell) { return '0'; }
				if (cell[2] == 'M') 
				{
					sort_val = '1';
				} 
				else if (cell[2] == 'G') 
				{
					sort_val = '2';
				} 
				else if (cell[2] == 'T') 
				{
					sort_val = '3';
				} 
				else 
				{
					sort_val = '0';
				}
				
				var i = cell[1].indexOf('.');
				if (i == -1) 
				{
					post = '00';
				} 
				else 
				{
					var dec = cell[1].split('.');
					cell[1] = dec[0];
					post = dec[1].concat('00'.substr(0,2-dec[1].length));
				}
				return sort_val.concat('00000000'.substr(0,2-cell[1].length).concat(cell[1])).concat(post);
			}
		},
      
		// 1 day ago, 4 days ago, 38 years ago, 1 month ago
		{ 
			matcher: /(\d{1,2}) (.{3,6}) ago/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				cell = this.conversion_matcher.exec( cell );
				if (!cell) { return '0'; }
				var sort_val;
				if (cell[2].indexOf('month') != -1) 
				{
					sort_val = '1';
				} 
				else if (cell[2].indexOf('year') != -1) 
				{
					sort_val = '2';
				} 
				else 
				{
					sort_val = '0';
				}
				return sort_val.concat('00'.substr(0,2-cell[1].length).concat(cell[1]));
			}
		},
		// Currency, or floating point with precision up to .00
		{ 
			matcher: /^[^\d]?((\d+|,\d{3})*(\.\d{1,2})?)$/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				cell = parseFloat(cell.replace(/^[^\d]/, "").replace(/,/g, ""));
				if (isNaN(cell)) { cell = 0; }
				cell = Math.round((cell * 100)).toString();
				return '00000000000000000000000000000000'.substr(0,32-cell.length).concat(cell);
			}
		},
		// YYYY-MM-DD, YYYY-m-d, YYYY/MM/DD, YYYY/m/d
		{ 
			matcher: /(\d{4})[\-\/](\d{1,2})[\-\/](\d{1,2})/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				var d = this.conversion_matcher.exec( cell );
				return d ? d[1]+
                 '00'.substr(0,2-d[2].length).concat(d[2])+
                 '00'.substr(0,2-d[3].length).concat(d[3]) : cell;
			}
		},
		// MM/DD/YYYY, MM-DD-YYYY, m/d/YYYY, m-d-YYYY
		{ 
			matcher: /(\d{1,2})[\-\/](\d{1,2})[\-\/](\d{4})/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				var d = this.conversion_matcher.exec( cell );
				return d ? d[3]+
                 '00'.substr(0,2-d[1].length).concat(d[1])+
                 '00'.substr(0,2-d[2].length).concat(d[2]) : cell;
			}
		},
		// 
		// Numbers
		{ 
			matcher: /^\d+$/,
			conversion_function: function( row ) 
			{
				var cell = document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
				return '00000000000000000000000000000000'.substr(0,32-cell.length).concat(cell);
			}
		},
		// Fallback
		{ 
			matcher: /.*/,
			conversion_function: function( row ) 
			{
				return document.id(row.row.getElementsByTagName('td')[this.sort_column]).get('text');
			}
		}]);
	}
});
 
SortingTable.removeAltClassRe = new RegExp('(^|\\s)alt(?:\\s|$)');
SortingTable.implement({ removeAltClassRe: SortingTable.removeAltClassRe });
 
SortingTable.stripe_table = function ( tr_elements ) {
  var counter = 0;
  tr_elements.each( function( tr ) {
    if ( !tr.hasClass('collapsed') ) counter++;
    tr.className = tr.className.replace( this.removeAltClassRe, '$1').clean();
    if (counter % 2) tr.addClass( 'alt' );
  });
};

SortingTable.toggleSelect = function(button, table)
{
	button = document.id(button);
	var table = document.id(table);
	var state = (button.get('text') == "Select All");
	
	table.getElements('tr').each(function(tr)
	{
		var checkbox = tr.getElement("input[type=checkbox]");
		if (checkbox) checkbox.checked = state;		
	});
	
	button.set('text', state ? "Deselect All" : "Select All");
};