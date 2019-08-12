-- Component Schema for glossary
--
-- Author: Stephen Omwony
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0
-- END Version 1.0

-- START Version 1.1
CREATE TABLE glossary
(
	glossary_id		int(10) not null auto_increment,
	term			varchar(200) not null,
	definition		longtext,
	display			tinyint(3) not null default 0,
	PRIMARY KEY(glossary_id)
) ENGINE=InnoDB;

-- END Version 1.1