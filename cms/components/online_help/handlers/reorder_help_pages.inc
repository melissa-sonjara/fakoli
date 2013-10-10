<?php
/**************************************************************

Copyright (c) 2012 Sonjara, Inc

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

Fakoli::using("online_help");

$help_book_id = checkNumeric($_GET["help_book_id"]);

if(!$help_book_id)
{
	Fakoli::end("Reorder help pages - unknown help book");	
}

$tx = new DataTransaction();

try
{
	foreach($_GET["help_page_id"] as $id => $sort_order)
	{
		checkNumeric($id);
		checkNumeric($sort_order);

		$helpPage = new HelpPage($id);
		$helpPage->joinTransaction($tx);

		if ($helpPage->help_book_id != $help_book_id)
		{
			throw new FakoliException("Data Mismatch");
		}
		
		$helpPage->filter = new InclusionFilter("sort_order");
		$helpPage->sort_order = $sort_order;
		$helpPage->save();
	}

	$tx->commit();
}
catch(Exception $e)
{
	$tx->rollback();
	Fakoli::end($e->getMessage());
}

?>