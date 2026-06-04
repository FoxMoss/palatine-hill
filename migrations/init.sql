CREATE TABLE "access_tokens" (
	"slack_id"	TEXT NOT NULL UNIQUE,
	"access_token"	TEXT NOT NULL,
	"name"	TEXT NOT NULL,
	"last_updated"	TEXT,
	PRIMARY KEY("slack_id")
);

CREATE TABLE "pitches" (
	"slack_id"	TEXT NOT NULL,
	"title"	TEXT NOT NULL,
	"explanation"	TEXT NOT NULL,
	"id"	INTEGER NOT NULL UNIQUE,
	"pitch_timestamp"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("slack_id") REFERENCES "access_tokens"("slack_id")
);
