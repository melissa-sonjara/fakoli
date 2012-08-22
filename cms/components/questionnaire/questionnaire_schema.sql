-- Fakoli Questionnaire Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `answer` (
  `answer_id` int(10) unsigned NOT NULL auto_increment,
   `question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
`composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

CREATE TABLE `question_type` (
  `question_type_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'type name',
  `class_name` varchar(100) NOT NULL COMMENT 'the name of the question renderer class',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'order in the question form drop down',
  `options` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type has an options list',
  `char_limit` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type can have a char limit; 0 if not',
  `num_rows` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'default number of rows, if applicable to type; 0 if not',
  PRIMARY KEY (`question_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Question types available for questionnaires';

-- Default question types
/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;
INSERT INTO `question_type` (`question_type_id`,`name`,`class_name`,`sort_order`,`options`,`char_limit`,`num_rows`) VALUES
 (1,'Multiple Choice','MultipleChoiceView',4,1,0,0),
 (2,'Rating Question','RatingView',5,1,0,0),
 (3,'Short Text','ShortTextView',2,0,0,0),
 (4,'Free Text','FreeTextView',1,0,1,3),
 (5,'Checklist','CheckListView',3,1,0,0),
 (6,'Drop Down List','SelectFieldView',6,1,0,0);
/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;

CREATE TABLE  `questionnaire` (
  `questionnaire_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  `composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`questionnaire_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='user-defined questionnaire';

INSERT INTO `questionnaire` (`questionnaire_id`,`title`) VALUES
  (1,'Sample Questionnaire');


CREATE TABLE `question` (
  `question_id` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) default NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY  (`question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';

INSERT INTO `question` (`questionnaire_id`,`question_type_id`,`question`,`question_number`,`question_name`,`options`,`required`,`char_limit`,`num_rows`) VALUES
  (1,4,'This is a Free Text question. The respondent is provided a text field for the answer. The answer may be several sentences. You can limit the number of characters in their answer if you wish and specify the height of the text box as the number of rows.\r\n\r\nFor example: Please let us know how we can improve our service.',3,'','',0,0,3),
 (1,3,'This is a Short Text question. Respondents will have just one line for their answer.',4,'','',0,0,3),
 (1,5,'This is a checklist question. You may require a minimum number of checkboxes to be checked. \r\n\r\nWhich of these factors influenced your decision to shop with us? ',5,'','price\r\nservice\r\nspeed of delivery',1,0,3),
 (1,1,'Multiple Choice question.\r\n\r\nHow did you hear about us?',6,'','web site\r\nfriend\r\nnewspaper',0,0,3),
 (1,2,'Rating Question. \r\n\r\nHow would you rate our service?',7,'','Lowest\nHighest\n5',0,0,3),
 (1,6,'Drop Down List.\r\n\r\nWhat is your age range?',8,'','10-15\r\n16-20\r\n21-25\r\n25-30\r\n31-35\r\n36-40\r\n41-45\r\n46-50\r\n51-55\r\n56-60\r\nover 60',0,0,3),
 (1,5,'Checklist Question with just one Yes checkbox:\r\n\r\nDo you agree to the terms stated above?',9,'','Yes',1,0,3);



-- END Version 1.0

-- START Version 1.1
 
ALTER TABLE `question` ADD COLUMN `context_help` TEXT;

-- END Version 1.1

-- START Version 1.2

/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;
INSERT INTO `question_type` (`question_type_id`,`name`,`class_name`,`sort_order`,`options`,`char_limit`,`num_rows`) VALUES
 (7,'Heading','HeadingView',7,1,0,0);
/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;

-- END Version 1.2