tinymce.PluginManager.add('imagepicker', function(editor, url) {
    // Add a button that opens a window
    editor.addButton('imagepicker', {
    	title: 'Insert Image',
        image: '/fakoli/tinymce/skins/fakoli/img/insertimage.png',
        onclick: function() {
        	imagePicker(editor);
        }
    });
});
