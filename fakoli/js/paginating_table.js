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
// new PaginatingTable( 'my_table', 'ul_for_paginating', {
// per_page: 10, // How many rows per page?
// current_page: 1, // What page to start on when initialized
// offset_el: false, // What dom element to stick the offset in
// cutoff_el: false, // What dom element to stick the cutoff in
// details: false // Do we have hidden/collapsable rows?
// });
//
// The above were the defaults. You could also pass an array of paginators
// instead of just one.
//
// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
 
var PaginatingTable = new Class({
 
  Implements: Options,
  
  options: {
    per_page: 10,
    current_page: 1,
    offset_el: false,
    cutoff_el: false,
    details: false,
    link_count: 10,
    zebra: false
  },
  
  initialize: function( table, ids, options ) {
    this.table = $(table);
    this.setOptions(options);
    
    this.tbody = this.table.getElement('tbody');
    
    if (this.options.offset_el)
      this.options.offset_el = $(this.options.offset_el);
    if (this.options.cutoff_el)
      this.options.cutoff_el = $(this.options.cutoff_el);
 
    this.paginators = ($type(ids) == 'array') ? ids.map($) : [$(ids)];
 
    if (this.options.details) {
      this.options.per_page = this.options.per_page * 2;
    }
 

	if (this.table.facetManager)
	{
		this.table.facetManager.addEvent('filterChanged', function() { this.filterChanged()}.bind(this));
		this.table.facetManager.addEvent('filterCleared', function() { this.filterCleared()}.bind(this));
		this.preprocessFacets();
	}

    this.update_pages();
  },
 
  countRows: function()
  {
	  var filtered = this.tbody.getElements(".filtered").length;
	  var all = this.tbody.getChildren().length;
	  return all - filtered;
  },
  
  update_pages: function(){
    this.pages = Math.ceil( this.countRows() / this.options.per_page );
    this.create_pagination();
    this.restripeTable();
    this.to_page( 1 );
  },
 
  to_page: function( page_num ) {
    page_num = page_num.toInt();
    if (page_num > this.pages || page_num < 1) return;
    this.current_page = page_num;
    this.low_limit = this.options.per_page * ( this.current_page - 1 );
    this.high_limit = this.options.per_page * this.current_page;
    var trs = this.tbody.getChildren();
    if (trs.length < this.high_limit) this.high_limit = trs.length;
    for (var i = 0, c = 0,j = trs.length; i < j; i++) {
    	if (trs[i].hasClass("filtered")) 
    	{
    		trs[i].style.display = 'none';
    		continue;
    	}
    
      trs[i].style.display = (this.low_limit <= c && this.high_limit > c) ? '' : 'none';
  	  c++;
    }
    this.paginators.each(function(paginator){
    	var pagers = paginator.getElements("a.goto-page");
    	pagers.each(function(p) { p.innerHTML = 'Page ' + (this.current_page || 1) + " of " + this.pages}.bind(this));    	
    }, this);
    if (this.options.offset_el)
      this.options.offset_el.set('text', Math.ceil( this.low_limit / ( this.options.details ? 2 : 1 ) + 1 ) );
    if (this.options.cutoff_el)
      this.options.cutoff_el.set('text', ( this.high_limit / ( this.options.details ? 2 : 1 ) ) );
  },
 
  to_next_page: function() {
    this.to_page( this.current_page + 1 );
  },
 
  to_prev_page: function() {
    this.to_page( this.current_page - 1 );
  },
 
  create_pagination: function() {
    this.paginators.each(function(paginator){
      paginator.empty();
      this.create_pagination_node( '&#60;&#60;&#32;&#80;&#114;&#101;&#118;', function(evt){
        var evt = new Event( evt );
        this.to_prev_page();
        evt.stop();
        return false;
      }).injectInside( paginator );
      
      var li = new Element('li', {'class': 'pager'} );
      var a = new Element('a', {'href': "#", 'class': 'goto-page', 'html': 'Page ' + (this.current_page || 1) + " of " + this.pages});
      a.injectInside(li);
      li.injectInside(paginator);
      
     this.create_pagination_node( '&#78;&#101;&#120;&#116;&#32;&#62;&#62;', function(evt){
        var evt = new Event( evt );
        this.to_next_page();
        evt.stop();
        return false;
      }).injectInside( paginator );
    }.bind( this ));
  },
 
  create_pagination_node: function( text, evt ) {
    var span = new Element( 'span' ).set( 'html', text );
    if (text == '&#60;&#60;&#32;&#80;&#114;&#101;&#118;'){
      var a = new Element( 'a', { 'href': '#', 'class': 'paginate' }).addEvent( 'click', evt.bind( this ) );
    } else if (text == '&#78;&#101;&#120;&#116;&#32;&#62;&#62;'){
      var a = new Element( 'a', { 'href': '#', 'class': 'paginate' }).addEvent( 'click', evt.bind( this ) );
    } else {
      var a = new Element( 'a', { 'href': '#'}).addEvent( 'click', evt.bind( this ) );
    }
    var li = new Element( 'li' );
    span.injectInside( a.injectInside( li ) );
    return li;
  },
  
    restripeTable: function () 
    {
	  if (!this.options.zebra) return;
	  
	  var tr_elements = this.tbody.getChildren('tr');
	  var counter = 0;
  
	  tr_elements.each( function( tr ) 
	  { 
		  if ( tr.hasClass('filtered')) return;
		  if ( !tr.hasClass('collapsed') ) counter++;
		  tr.removeClass('alt');
		  if (counter % 2) tr.addClass( 'alt' );
	  });
    },
  
	preprocessFacets: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			this.table.facetManager.preprocess(elt);
		}.bind(this));
		
		this.table.facetManager.preprocessComplete();
	},
	
	filterChanged: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.table.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
			}
			else
			{
				elt.addClass('filtered');
			}
		}.bind(this));
		
 	    this.update_pages();
	},
	
	filterCleared: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
		});
		
  	    this.update_pages();
	}
 
});
 