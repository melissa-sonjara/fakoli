<?php
/**\addtogroup survey
 * @{
 */


/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

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

/*
 * Defines a set of questions to be used in a survey (if survey_id)
 * or a standard question set defined by admin that can be used
 * in a survey, if selected (if no survey_id and name is not null).
 *
 */
class SurveyQuestionXref extends DataItem
{
	var $fields = array(
				"survey_question_xref_id"	=>	Number,
				"survey_id"					=>	Number,
				"survey_question_id"		=>	Number,
				"sort_order"				=>	Number
	);

	var $relations = array(
				"Survey"			=>	Survey,
				"Question"			=>	SurveyQuestion
	);

	function Question()
	{
		return $this->getRelated(SurveyQuestion);
	}

	function Survey()
	{
		if($this->survey_id)
			return $this->getRelated(Survey);
	}

	/*
	 * get sort order either by searching for max with same survey_id
	 */
	function getDefaultSortOrder()
	{
		if(!$this->survey_id)
			return 1;
			
		return Query::create(SurveyQuestionXref, "WHERE survey_id=:survey_id")
			->bind(":survey_id", $this->survey_id)
			->executeValue("MAX(sort_order)") + 1;
	}

	function saveSortOrder($value)
	{
		$this->filter = new InclusionFilter("sort_order");
		$this->sort_order = $value;
		$this->save();
	}

	function SurveyQuestionXref()
	{
		$this->table = "survey_question_xref";
		$this->primary_key = "survey_question_xref_id";

		$this->DataItem(func_get_args());

	}
}
/**@}*/?>