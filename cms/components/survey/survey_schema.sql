-- Fakoli Survey Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `survey` (
  `survey_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `start_date` date DEFAULT NULL COMMENT 'date survey request sent',
  `end_date` date DEFAULT NULL COMMENT 'date survey is closed',
  `user_id` int(10) unsigned NOT NULL COMMENT 'references site_user table',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `message` text,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `deleted` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Links a questionnaire to an email template';

CREATE TABLE `survey_answer` (
  `survey_answer_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `response_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire_response table',
  `survey_question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY (`survey_answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

CREATE TABLE `survey_question` (
  `survey_question_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) DEFAULT NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned DEFAULT '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned DEFAULT '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned DEFAULT '0' COMMENT 'number of input rows to display on freetext',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this is a standard, multi-reuse question',
  PRIMARY KEY (`survey_question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';

CREATE TABLE `survey_question_xref` (
  `survey_question_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned DEFAULT NULL COMMENT 'if survey question set',
  `survey_template_id` int(10) unsigned DEFAULT NULL COMMENT 'if standard question set',
  `survey_question_id` int(10) unsigned NOT NULL,
  `sort_order` tinyint(4) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_question_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='survey question set or standard set';


CREATE TABLE `survey_response` (
  `response_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `token` varchar(10) NOT NULL COMMENT 'assigned response token',
  `email` varchar(100) NOT NULL,
  `last_modified` date DEFAULT NULL COMMENT 'date submitted',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`response_id`),
  UNIQUE KEY `token_idx` (`token`,`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `survey_template` (
  `survey_template_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `message` text NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'in progress, published, in review',
  PRIMARY KEY (`survey_template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
