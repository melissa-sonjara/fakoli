<?php
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

Fakoli::usingFeature("data_view", "join");

class DocumentTotalDownloadsView
{
	var $library;
	var $table;
	
	function DocumentTotalDownloadsView($library)
	{
		$this->library = $library;

		$join = new InnerJoin();
		$join->add(Document);
		$join->add(DocumentDownloadTotalStats);
		$results = $join->query("WHERE document_library_id={$this->library->document_library_id} ORDER BY title");
		
		$this->table = new DataListView($results, "document_download_stats_total");
		$this->table->column("File", "<a href='document_form?document_id={Document.document_id}'>{Document.title}</a>", true, "width: 80%")
					->column("# Downloads", "{DocumentDownloadTotalStats.download_count:0}", true, "width: 20%; text-align: right");
					
		$this->table->filter = true;
		$this->table->pageSize = 25;
		$this->table->excelFile = $library->identifier."_download_stats_".date("Y_m_d").".xls";
		$this->table->emptyMessage = "No documents have been downloaded from this library";
		
		$this->table->footerText("Total")
			 		->footerTotal("{DocumentDownloadTotalStats.download_count:0}", "text-align: right");
	}
	
	function writeScript()
	{
		return $this->table->writeScript();
	}
	
	function drawView()
	{
		$this->table->drawView();
	}
}
?>