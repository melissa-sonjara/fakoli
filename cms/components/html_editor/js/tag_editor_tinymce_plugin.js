tinymce.PluginManager.add('tag_editor', function(editor, url) {
    // Add a button that opens a window
    editor.addButton('tag_editor', {
    	title: 'Edit HTML Tag',
        image: '/components/html_editor/images/tag_editor.png',
        onclick: function() {
        	new TagEditor().showTagEditor(editor);
        }
    });

    editor.addMenuItem('tag_editor', {
    	text: 'Edit HTML Tag',
        image: '/components/html_editor/images/tag_editor.png',
        onclick: function() {
        	new TagEditor().showTagEditor(editor);
        }
    });

});