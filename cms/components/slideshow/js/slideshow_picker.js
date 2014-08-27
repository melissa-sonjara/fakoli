var SlideshowPicker = new Class(
{
	slideshow_id: 0,
	table: null,
	editor: "",
	
	initialize: function(table, editor)
	{
		this.table = document.id(table);
		this.editor = editor;
	},
	
	selectSlideshow: function(link, id)
	{
		this.table.getElements('tr').each(function(row) { row.removeClass('selected'); });
		var tr = findAncestor(link, "tr");
		tr.addClass('selected');
		
		this.slideshow_id = id;
	},
	
	insertSlideshow: function()
	{
		var width = document.id('width').value;
		var height = document.id('height').value;
		
		if (!width) width = 500;
		if (!height) height = 400;
		
		var slideshow = "<div>{slideshow:" + this.slideshow_id + ":" + width + "x" + height + "}</div>";
		
		var editor = window.opener.theEditors[this.editor];
		editor.insertAtSelection(slideshow);
		window.close();
	}
});