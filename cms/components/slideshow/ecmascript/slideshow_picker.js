class SlideshowPicker
{
	constructor(table, editor)
	{
		this.slideshow_id = 0;
		this.table = document.getElementById(table);
		this.editor = editor;
	}

	selectSlideshow(link, id)
	{
		this.table.querySelectorAll('tr').forEach(function(row) { row.classList.remove('selected'); });
		var tr = findAncestor(link, "tr");
		tr.classList.add('selected');

		this.slideshow_id = id;
	}

	insertSlideshow()
	{
		var width = document.getElementById('width').value;
		var height = document.getElementById('height').value;

		if (!width) width = 500;
		if (!height) height = 400;

		var slideshow = "<div>{slideshow:" + this.slideshow_id + ":" + width + "x" + height + "}</div>";

		var editor = window.opener.theEditors[this.editor];
		editor.insertAtSelection(slideshow);
		window.close();
	}
}
