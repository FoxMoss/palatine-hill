CREATE TABLE "votes" (
	"id"	INTEGER NOT NULL UNIQUE,
	"slack_id"	TEXT NOT NULL,
	"project_id"	INTEGER NOT NULL,
	"time_created"	TEXT,
	"message_timestamp"	TEXT NOT NULL UNIQUE,
	CONSTRAINT "project_id" FOREIGN KEY("project_id") REFERENCES "pitches"("id"),
	CONSTRAINT "slack_id" FOREIGN KEY("slack_id") REFERENCES "access_tokens"("slack_id"),
  CONSTRAINT vote_signature UNIQUE (slack_id, project_id),
	PRIMARY KEY("id" AUTOINCREMENT)
);
