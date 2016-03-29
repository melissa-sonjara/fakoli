tinymce.PluginManager.add('snippet', function(editor, url) {
    // Add a button that opens a window
    editor.addButton('snippet', {
    	title: 'Insert Snippet',
        image: '/components/html_editor/images/snippet.png',
        onclick: function() {
        	new SnippetManager().showPicker(editor);
        }
    });
    

    editor.addMenuItem('snippet', {
    	text: 'Insert Snippet',
    	icon:  'chart',
        image: '/components/html_editor/images/snippet.png',
        onclick: function() {
        	new SnippetManager().showPicker(editor);
        }
    });

});
