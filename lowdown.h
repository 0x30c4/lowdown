/*	$Id$ */
/*
 * Copyright (c) 2017 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef LOWDOWN_H
#define LOWDOWN_H

/*
 * All of this is documented in lowdown.3.
 * If it's not documented, don't use it.
 * Or report it as a bug.
 */

/* We need this for compilation on musl systems. */

#ifndef __BEGIN_DECLS
# ifdef __cplusplus
#  define       __BEGIN_DECLS           extern "C" {
# else
#  define       __BEGIN_DECLS
# endif
#endif
#ifndef __END_DECLS
# ifdef __cplusplus
#  define       __END_DECLS             }
# else
#  define       __END_DECLS
# endif
#endif

enum	lowdown_type {
	LOWDOWN_HTML,
	LOWDOWN_LATEX,
	LOWDOWN_MAN,
	LOWDOWN_NROFF,
	LOWDOWN_TERM,
	LOWDOWN_TREE,
	LOWDOWN_NULL
};

enum	lowdown_err {
	LOWDOWN_ERR_SPACE_BEFORE_LINK = 0,
	LOWDOWN_ERR_METADATA_BAD_CHAR,
	LOWDOWN_ERR_UNKNOWN_FOOTNOTE,
	LOWDOWN_ERR_DUPE_FOOTNOTE,
	LOWDOWN_ERR__MAX
};

/*
 * All types of Markdown nodes that lowdown understands.
 */
enum	lowdown_rndrt {
	LOWDOWN_ROOT,
	LOWDOWN_BLOCKCODE,
	LOWDOWN_BLOCKQUOTE,
	LOWDOWN_DEFINITION,
	LOWDOWN_DEFINITION_TITLE,
	LOWDOWN_DEFINITION_DATA,
	LOWDOWN_HEADER,
	LOWDOWN_HRULE,
	LOWDOWN_LIST,
	LOWDOWN_LISTITEM,
	LOWDOWN_PARAGRAPH,
	LOWDOWN_TABLE_BLOCK,
	LOWDOWN_TABLE_HEADER,
	LOWDOWN_TABLE_BODY,
	LOWDOWN_TABLE_ROW,
	LOWDOWN_TABLE_CELL,
	LOWDOWN_FOOTNOTES_BLOCK,
	LOWDOWN_FOOTNOTE_DEF,
	LOWDOWN_BLOCKHTML,
	LOWDOWN_LINK_AUTO,
	LOWDOWN_CODESPAN,
	LOWDOWN_DOUBLE_EMPHASIS,
	LOWDOWN_EMPHASIS,
	LOWDOWN_HIGHLIGHT,
	LOWDOWN_IMAGE,
	LOWDOWN_LINEBREAK,
	LOWDOWN_LINK,
	LOWDOWN_TRIPLE_EMPHASIS,
	LOWDOWN_STRIKETHROUGH,
	LOWDOWN_SUPERSCRIPT,
	LOWDOWN_FOOTNOTE_REF,
	LOWDOWN_MATH_BLOCK,
	LOWDOWN_RAW_HTML,
	LOWDOWN_ENTITY,
	LOWDOWN_NORMAL_TEXT,
	LOWDOWN_DOC_HEADER,
	LOWDOWN_META,
	LOWDOWN_DOC_FOOTER,
	LOWDOWN__MAX
};

typedef struct hbuf {
	char		*data;	/* actual character data */
	size_t		 size;	/* size of the string */
	size_t		 asize;	/* allocated size (0 = volatile) */
	size_t		 unit;	/* realloc unit size (0 = read-only) */
	int 		 buffer_free; /* obj should be freed */
} hbuf;

TAILQ_HEAD(lowdown_nodeq, lowdown_node);

enum 	htbl_flags {
	HTBL_FL_ALIGN_LEFT = 1,
	HTBL_FL_ALIGN_RIGHT = 2,
	HTBL_FL_ALIGN_CENTER = 3,
	HTBL_FL_ALIGNMASK = 3,
	HTBL_FL_HEADER = 4
};

enum 	halink_type {
	HALINK_NONE, /* used internally when it is not an autolink */
	HALINK_NORMAL, /* normal http/http/ftp/mailto/etc link */
	HALINK_EMAIL /* e-mail link without explit mailto: */
};

enum	hlist_fl {
	HLIST_FL_ORDERED = (1 << 0), /* <ol> list item */
	HLIST_FL_BLOCK = (1 << 1), /* <li> containing block data */
	HLIST_FL_UNORDERED = (1 << 2), /* <ul> list item */
	HLIST_FL_DEF = (1 << 3) /* <dl> list item */
};

/*
 * Meta-data keys and values.
 * Both of these are non-NULL (but possibly empty).
 */
struct	lowdown_meta {
	char		*key;
	char		*value;
	TAILQ_ENTRY(lowdown_meta) entries;
};

TAILQ_HEAD(lowdown_metaq, lowdown_meta);

enum	lowdown_chng {
	LOWDOWN_CHNG_NONE = 0,
	LOWDOWN_CHNG_INSERT,
	LOWDOWN_CHNG_DELETE,
};

/*
 * Node parsed from input document.
 * Each node is part of the parse tree.
 */
struct	lowdown_node {
	enum lowdown_rndrt	 type;
	enum lowdown_chng	 chng; /* change type */
	size_t			 id; /* unique identifier */
	union {
		struct rndr_meta {
			hbuf key;
		} rndr_meta;
		struct rndr_list {
			/*
			 * This should only be checked for bit-wise
			 * HLIST_FL_ORDERED OR HLIST_FL_BLOCK.
			 * There may be other private bits set.
			 */
			enum hlist_fl flags;
			/*
			 * This is string of size >0 iff
			 * HLIST_FL_ORDERED and we're parsing
			 * CommonMark, else it's an empty string.
			 */
			char start[10];
		} rndr_list; 
		struct rndr_paragraph {
			size_t lines; /* input lines */
			int beoln; /* ends on blank line */
		} rndr_paragraph;
		struct rndr_listitem {
			enum hlist_fl flags; /* all possible flags */
			size_t num; /* index in ordered */
		} rndr_listitem; 
		struct rndr_header {
			size_t level; /* hN level */
		} rndr_header; 
		struct rndr_normal_text {
			hbuf text; /* basic text */
			size_t offs; /* in-render offset */
		} rndr_normal_text; 
		struct rndr_entity {
			hbuf text; /* entity text */
		} rndr_entity; 
		struct rndr_autolink {
			hbuf link; /* link address */
			hbuf text; /* shown address */
			enum halink_type type; /* type of link */
		} rndr_autolink; 
		struct rndr_raw_html {
			hbuf text; /* raw html buffer */
		} rndr_raw_html; 
		struct rndr_link {
			hbuf link; /* link address */
			hbuf title; /* title of link */
		} rndr_link; 
		struct rndr_blockcode {
			hbuf text; /* raw code buffer */
			hbuf lang; /* fence language */
		} rndr_blockcode; 
		struct rndr_definition {
			enum hlist_fl flags;
		} rndr_definition; 
		struct rndr_codespan {
			hbuf text; /* raw code buffer */
		} rndr_codespan; 
		struct rndr_table_header {
			enum htbl_flags *flags; /* per-column flags */
			size_t columns; /* number of columns */
		} rndr_table_header; 
		struct rndr_table_cell {
			enum htbl_flags flags; /* flags for cell */
			size_t col; /* column number */
			size_t columns; /* number of columns */
		} rndr_table_cell; 
		struct rndr_footnote_def {
			size_t num; /* footnote number */
		} rndr_footnote_def;
		struct rndr_footnote_ref {
			size_t num; /* footnote number */
		} rndr_footnote_ref;
		struct rndr_image {
			hbuf link; /* image address */
			hbuf title; /* title of image */
			hbuf dims; /* dimensions of image */
			hbuf alt; /* alt-text of image */
#if 0
			hbuf attr_width; 
#endif
		} rndr_image;
		struct rndr_math {
			hbuf text; /* equation (opaque) */
			int blockmode;
		} rndr_math;
		struct rndr_blockhtml {
			hbuf text;
		} rndr_blockhtml;
	};
	struct lowdown_node *parent;
	struct lowdown_nodeq children;
	TAILQ_ENTRY(lowdown_node) entries;
};

/*
 * These options contain everything needed to parse and render content.
 */
struct	lowdown_opts {
	enum lowdown_type	 type;
	size_t			 maxdepth; /* max parse tree depth */
	size_t			 cols; /* -Tterm width */
	size_t			 hmargin; /* -Tterm left margin */
	size_t			 vmargin; /* -Tterm top/bot margin */
	unsigned int		 feat;
#define LOWDOWN_TABLES		 0x01
#define LOWDOWN_FENCED		 0x02
#define LOWDOWN_FOOTNOTES	 0x04
#define LOWDOWN_AUTOLINK	 0x08
#define LOWDOWN_STRIKE		 0x10
/* Omitted 			 0x20 */
#define LOWDOWN_HILITE		 0x40
/* Omitted 			 0x80 */
#define LOWDOWN_SUPER		 0x100
#define LOWDOWN_MATH		 0x200
#define LOWDOWN_NOINTEM		 0x400
/* Disabled LOWDOWN_MATHEXP	 0x1000 */
#define LOWDOWN_NOCODEIND	 0x2000
#define	LOWDOWN_METADATA	 0x4000
#define	LOWDOWN_COMMONMARK	 0x8000
#define	LOWDOWN_DEFLIST		 0x10000
	unsigned int		 oflags;
#define LOWDOWN_HTML_SKIP_HTML	 0x01 /* skip all HTML */
#define LOWDOWN_HTML_ESCAPE	 0x02 /* escape HTML (if not skip) */
#define LOWDOWN_HTML_HARD_WRAP	 0x04 /* paragraph line breaks */
#define LOWDOWN_NROFF_SKIP_HTML	 0x08 /* skip all HTML */
#define LOWDOWN_NROFF_HARD_WRAP	 0x10 /* paragraph line breaks */
#define LOWDOWN_NROFF_GROFF	 0x20 /* use groff extensions */
#define	LOWDOWN_SMARTY	  	 0x40 /* smart typography */
#define LOWDOWN_NROFF_NUMBERED	 0x80 /* numbered section headers */
#define LOWDOWN_HTML_HEAD_IDS	 0x100 /* <hN id="the_name"> */
#define LOWDOWN_STANDALONE	 0x200 /* emit complete document */
#define LOWDOWN_TERM_SHORTLINK	 0x400 /* shorten URLs */
#define	LOWDOWN_HTML_OWASP	 0x800 /* use OWASP escaping */
#define	LOWDOWN_HTML_NUM_ENT	 0x1000 /* use &#nn; if possible */
#define LOWDOWN_LATEX_SKIP_HTML	 0x2000 /* skip all HTML */
};

struct lowdown_doc;

__BEGIN_DECLS

/*
 * High-level functions.
 * These use the "lowdown_opts" to determine how to parse and render
 * content, and extract that content from a buffer, file, or descriptor.
 */
void	 lowdown_buf(const struct lowdown_opts *, 
		const char *, size_t,
		char **, size_t *, struct lowdown_metaq *);
void	 lowdown_buf_diff(const struct lowdown_opts *, 
		const char *, size_t, const char *, size_t,
		char **, size_t *, struct lowdown_metaq *);
int	 lowdown_file(const struct lowdown_opts *, 
		FILE *, char **, size_t *, struct lowdown_metaq *);
int	 lowdown_file_diff(const struct lowdown_opts *, FILE *, 
		FILE *, char **, size_t *, struct lowdown_metaq *);

/* 
 * Low-level functions.
 * These actually parse and render the AST from a buffer in various
 * ways.
 */

struct lowdown_doc
	*lowdown_doc_new(const struct lowdown_opts *);
struct lowdown_node
	*lowdown_doc_parse(struct lowdown_doc *,
		size_t *, const char *, size_t);
struct lowdown_node
	*lowdown_diff(const struct lowdown_node *,
		const struct lowdown_node *, size_t *);
void	 lowdown_doc_free(struct lowdown_doc *);
void	 lowdown_metaq_free(struct lowdown_metaq *);

void 	 lowdown_node_free(struct lowdown_node *);

void	 lowdown_html_free(void *);
void	*lowdown_html_new(const struct lowdown_opts *);
void 	 lowdown_html_rndr(hbuf *, struct lowdown_metaq *,
		void *, const struct lowdown_node *);

void	 lowdown_term_free(void *);
void	*lowdown_term_new(const struct lowdown_opts *);
void 	 lowdown_term_rndr(hbuf *, struct lowdown_metaq *,
		void *, const struct lowdown_node *);

void	 lowdown_nroff_free(void *);
void	*lowdown_nroff_new(const struct lowdown_opts *);
void 	 lowdown_nroff_rndr(hbuf *, struct lowdown_metaq *,
		void *, struct lowdown_node *);

void	 lowdown_tree_free(void *);
void	*lowdown_tree_new(void);
void 	 lowdown_tree_rndr(hbuf *, struct lowdown_metaq *,
		void *, const struct lowdown_node *);

void	 lowdown_latex_free(void *);
void	*lowdown_latex_new(const struct lowdown_opts *);
void 	 lowdown_latex_rndr(hbuf *, struct lowdown_metaq *,
		void *, const struct lowdown_node *);

__END_DECLS

#endif /* !LOWDOWN_H */
