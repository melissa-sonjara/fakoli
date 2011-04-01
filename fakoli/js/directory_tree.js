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

var DirectoryTree = new Class({
	
	initialize: function()
	{
	}
});

DirectoryTree.toggleDirectoryFolder = function(id, openStyle, closedStyle)
{
	var div = $(id + "_contents");
	var link = $(id);
	var toggle = $(id + "_toggle");
	var folder = $(id + "_folder");

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.removeClass(closedStyle);
		link.addClass(openStyle);
		toggle.src = "<?echo $this->images['toggle_open']?>";
		folder.src = "<?echo $this->images['folder_open']?>";
	}
	else
	{
		div.style.display="none";
		link.removeClass(openStyle);
		link.addClass(closedStyle);
		toggle.src = "<?echo $this->images['toggle_closed']?>";
		folder.src = "<?echo $this->images['folder_closed']?>";
	}
};

DirectoryTree.selectItem = function(treeID, itemID, itemValue)
{
	DirectoryTree._clearSelectionImpl(treeID);
	var hidden = $(treeID + "_value");
	hidden.value = itemValue;

	var node = $(treeID + "_node");
	node.value = itemID;
	
	var item = $(itemID + "_link");
	item.addClass("selected");
	eval(treeID + "_onSelectItem('" + itemID + "', '" + itemValue + "');");
};

DirectoryTree.clearSelection = function(treeID)
{

	DirectoryTree._clearSelectionImpl(treeID);
	eval(treeID + "_onClearSelection();");
};

DirectoryTree._clearSelectionImpl = function(treeID)
{
	var hidden = $(treeID + "_value");
	hidden.value = "";
	var tree = $(treeID);
	var elts = tree.getElementsByTagName("a");
	for (i = 0; i < elts.length; ++i)
	{
		elts[i].removeClass("selected");
	}
}
