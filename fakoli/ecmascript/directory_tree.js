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

var DirectoryTree = {};

DirectoryTree.toggleDirectoryFolder = function(id, openStyle, closedStyle)
{
	var div    = $el(id + "_contents");
	var link   = $el(id);
	var toggle = $el(id + "_toggle");
	var folder = $el(id + "_folder");

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.classList.remove(closedStyle);
		link.classList.add(openStyle);
		toggle.src = "/fakoli/images/toggle_open.png";
		folder.src = "/fakoli/images/folder_open.png";
	}
	else
	{
		div.style.display = "none";
		link.classList.remove(openStyle);
		link.classList.add(closedStyle);
		toggle.src = "/fakoli/images/toggle_closed.png";
		folder.src = "/fakoli/images/folder_closed.png";
	}
};

DirectoryTree.selectItem = function(treeID, itemID, itemValue)
{
	DirectoryTree._clearSelectionImpl(treeID);
	$el(treeID + "_value").value = itemValue;
	$el(treeID + "_node").value = itemID;
	$el(itemID + "_link").classList.add("selected");

	var onSelect = window[treeID + "_onSelectItem"];
	if (typeof onSelect === 'function') onSelect(itemID, itemValue);
};

DirectoryTree.clearSelection = function(treeID)
{
	DirectoryTree._clearSelectionImpl(treeID);
	var onClear = window[treeID + "_onClearSelection"];
	if (typeof onClear === 'function') onClear();
};

DirectoryTree._clearSelectionImpl = function(treeID)
{
	$el(treeID + "_value").value = "";
	var tree = $el(treeID);
	Array.from(tree.querySelectorAll("a")).forEach(function(a)
	{
		a.classList.remove("selected");
	});
};
