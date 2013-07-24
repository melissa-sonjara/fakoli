-- Component Schema for bookmark
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE bookmark (
	bookmark_id	INT(10) UNSIGNED AUTO_INCREMENT,
	category 	VARCHAR(200) NOT NULL DEFAULT '',
	title	 	VARCHAR(500) NOT NULL,
	url			VARCHAR(2000) NOT NULL,
	user_id		INT(1) UNSIGNED NOT NULL,
	PRIMARY KEY(bookmark_id)
) ENGINE=InnoDB;

-- END Version 1.0