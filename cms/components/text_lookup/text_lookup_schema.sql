-- Fakoli Text Lookup Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE text_lookup (
  text_id int(10) unsigned NOT NULL auto_increment,
  code varchar(45) NOT NULL,
  text text NOT NULL,
  class_name varchar(50) NOT NULL COMMENT 'class name that can access merge code values, if used',
  PRIMARY KEY  (text_id)
) ENGINE=InnoDB;

CREATE TABLE  text_translation (
  text_translation_id int(10) unsigned NOT NULL auto_increment,
  text_id int(10) unsigned NOT NULL COMMENT 'references text_lookup table',
  language varchar(50) NOT NULL COMMENT 'references language table',
  text text NOT NULL,
  PRIMARY KEY  (text_translation_id)
) ENGINE=InnoDB;


-- END Version 1.0

-- START Version 1.1

ALTER TABLE text_lookup ADD COLUMN enabled TINYINT(3) NOT NULL DEFAULT 1;

-- END Version 1.1