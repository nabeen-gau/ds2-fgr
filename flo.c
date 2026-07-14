#include "common.c"

BinaryReader br = {0};

typedef struct {
	// header
	size_t spriteSetsOffset;
	size_t offset2; // same as nodesOffset?
	size_t nodesOffset;
	size_t animatorsOffset;
	size_t rootsOffset;
	size_t texturesOffset;
	size_t stringsOffset;
	size_t textsOffset;
	// sprite sets
	Dictzuzu spriteSet_offsets;
	// textures
	DictPairPair texturesOffsetsOffset;
	DictPairzu texRegionOffsetsOffset;
	// node -> animators, nodeListA, nodeListB
	Dictzuzu animatorOffsets;
	Dictzuzu nodeListAOffsets;
	Dictzuzu nodeListBOffsets;
	// nodeListA -> nodeItemsA
	Dictzuzu nodeItemsAOffsets;
	// nodeListB -> nodeItemsB
	Dictzuzu nodeItemsBOffsets;
	// animator -> frames
	Dictzuzu framesOffsets;
	Dictzuzu animatorOffsetOffsets;
	// nodes [id] -> [offset]
	Dictzuzu node_id_offsets;
	// texture -> name_offsets
	Dictstrzu texture_names;
	// texture [id] -> [offset]
	Dictzuzu texture_id_offsets;
} FLO_Writer;

FLO_Writer writer = {0};

#define sb_patch_offset(sb, pos)\
	do {\
		size_t _cur_pos_ = (sb)->count;\
		(sb)->count = (pos);\
		sb_write_varint((sb), _cur_pos_);\
		(sb)->count = _cur_pos_;\
	} while(0)

#define sb_patch_offset_value(sb, pos, value)\
	do {\
		size_t _cur_pos_ = (sb)->count;\
		(sb)->count = (pos);\
		sb_write_varint((sb), (value));\
		(sb)->count = _cur_pos_;\
	} while(0)

#define Varint int64_t
#define Assert assert
#define byte uint8_t
#define VARINT_LONG
#define ubyte byte

#define sb_write_V2f sb_write_v2f
#define sb_write_ubyte sb_append
#define sb_write_byte sb_append

#define FSeek(offs) br.position = (offs)

#define da_read(da, n, f) \
	do {\
		da_init((da), (n));\
		for (size_t i=0; i<(size_t)(n); i++) {\
			da_append((da), f());\
		}\
	} while (0)

#define da_read_arg(da, n, f, arg) \
	do {\
		da_init((da), (n));\
		for (size_t i=0; i<(size_t)(n); i++) {\
			da_append((da), f(arg));\
		}\
	} while (0)

#define da_read_null(da, n, f, arg) \
	do {\
		da_init((da), (n));\
		for (size_t i=0; i<(size_t)(n); i++) {\
			f(arg);\
		}\
	} while (0)

#define sb_write_Varint sb_write_varint
#define sb_write_short sb_write_int16t
#define sb_write_int sb_write_int32t

typedef struct {
    int unk00;
    // Many fields like this appear to just be padding in 64-bit,
    // but they sometimes have pretty suspicious values
    // so I can't be completely certain.
#ifdef VARINT_LONG
	int unk04;
#endif
    Varint spriteSetsOffset;
    Varint offset2;
    Varint nodesOffset;
    Varint animatorsOffset;
    Varint rootsOffset;
    Varint texturesOffset;
    Varint stringsOffset;
    Varint textsOffset;
    short spriteSetCount;
    short count2;
    short nodeCount;
    short animatorCount;
    short rootCount;
    short textureCount;
    short stringCount;
    short textCount;
    byte bigEndian;
    byte unk59;
    byte unk5A;
    byte unk5B;
    int unk5C;
    int unk60;
    int unk64;
    int unk68;
    int unk6C;
    int unk70;
    int unk74;
    int unk78;
    int unk7C;
    int unk80;
    int unk84;
    int unk88;
    int unk8C;
    int unk90;
    int unk94;
    int unk98;
    int unk9C;
    int unkA0;
#ifdef VARINT_LONG
	int unkA4;
#endif
} Header;

typedef struct {
    V2f v1;
    V2f v2;
} TexRegion;

void write_texregion(String_Builder *sb, const TexRegion *tr) {
	sb_write_v2f(sb, tr->v1);
	sb_write_v2f(sb, tr->v2);
}

// NOT NECESSARY
void tex_region_free(TexRegion *tr) {
	UNUSED(tr);
}

TexRegion read_tex_region() {
	return br_read(&br, TexRegion);
}

typedef struct {
	Varint name_offset;
    short id;
    short unk0A; //Assert(unk0A == 0);
    short unk0C; //Assert(unk0C == 0);
    short width; // These don't seem to actually do anything
    short height;
    short unk12; //Assert(unk12 == 0);
#ifdef VARINT_LONG
	int unk14;
#endif
} TextureHeader;

typedef struct {
	const char *name;
	TextureHeader texture;
} Texture;

// NOT NECESSARY
void texture_free(Texture *t) {
	UNUSED(t);
}

void write_texture(String_Builder *sb, const Texture *t) {
	*ht_put(&writer.texture_names, t->name) = sb->count;
	sb_write_Varint(sb, -1);
    sb_write_short(sb, t->texture.id);
    sb_write_short(sb, t->texture.unk0A);
    sb_write_short(sb, t->texture.unk0C);
    sb_write_short(sb, t->texture.width);
    sb_write_short(sb, t->texture.height);
    sb_write_short(sb, t->texture.unk12);
#ifdef VARINT_LONG
	sb_write_int(sb, t->texture.unk14);
#endif
}

DA(Texture, DATexture);

typedef struct {
    V2f position;
    V2f scale;
    V2f skew;
    int unk18;// Assert(unk18 == -1);
#ifdef VARINT_LONG
        int pad1C; //Assert(pad1C == 0);
#endif
    Varint textureOffset;
    short textureID;
    short unk2A; //Assert(unk2A == 0);
#ifdef VARINT_LONG
        int pad2C; //Assert(pad2C == 0);
#endif
    Varint texRegionOffset;
    int unk38; //Assert(unk38 == 0);
    int unk3C; //Assert(unk3C == 0);
} SpriteHeader;

typedef struct {
	SpriteHeader header;
	size_t texture_idx;
    TexRegion texRegion;
} Sprite;

void sprite_free(Sprite *s) {
	tex_region_free(&s->texRegion);
}

void write_sprite(String_Builder *sb, const Sprite *s, Pair ij) {
	sb_write_v2f(sb, s->header.position);
	sb_write_v2f(sb, s->header.scale);
	sb_write_v2f(sb, s->header.skew);
	sb_write_int32t(sb, s->header.unk18);
#ifdef VARINT_LONG
	sb_write_int32t(sb, s->header.pad1C);
#endif
	*ht_put(&writer.texturesOffsetsOffset, ij) = (Pair){s->header.textureID, sb->count};
    sb_write_varint(sb, -1);
    sb_write_int16t(sb, s->header.textureID);
    sb_write_int16t(sb, s->header.unk2A);
#ifdef VARINT_LONG
	sb_write_int32t(sb, s->header.pad2C);
#endif
	*ht_put(&writer.texRegionOffsetsOffset, ij) = sb->count;
    sb_write_varint(sb, -1);
    sb_write_int32t(sb, s->header.unk38);
    sb_write_int32t(sb, s->header.unk3C);
}

DA(Sprite, DASprite);

typedef struct {
    short id;
    short spriteCount;
#ifdef VARINT_LONG
	int pad04; //Assert(pad04 == 0);
#endif
    Varint spritesOffset;
    int unk10; //Assert(unk10 == 0);
    int unk14; //Assert(unk14 == 0);
} SpriteSetHeader;

typedef struct {
	SpriteSetHeader header;
	DASprite sprites;
} SpriteSet;

void sprite_set_free(SpriteSet *ss) {
	da_foreach(Sprite, s, &ss->sprites) {
		sprite_free(s);
	}
	da_free(ss->sprites);
}

void write_sprite_set(String_Builder *sb, const SpriteSet *ss, size_t idx) {
	sb_write_int16t(sb, ss->header.id);
	sb_write_int16t(sb, ss->sprites.count);
#ifdef VARINT_LONG
	sb_write_int32t(sb, ss->header.pad04);
#endif
	*ht_put(&writer.spriteSet_offsets, idx) = sb->count;
	sb_write_int32t(sb, -1);
	sb_write_int32t(sb, ss->header.unk10);
	sb_write_int32t(sb, ss->header.unk14);
}

typedef struct {
    int unk00;
    short unk04;
    short unk06;
    int unk08;
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
    int unk14; //Assert(unk14 == 0);
    int unk18; //Assert(unk18 == 0);
    int unk1C; //Assert(unk1C == 0);
} NodeItemB;

DA(NodeItemB, DANodeItemB);

// NOT NECESSARY
void node_item_b_free(NodeItemB *nib) {
	UNUSED(nib);
}

void write_node_item_b(String_Builder *sb, NodeItemB *nib) {
    sb_write_int(sb, nib->unk00);
    sb_write_short(sb, nib->unk04);
    sb_write_short(sb, nib->unk06);
    sb_write_int(sb, nib->unk08);
    sb_write_int(sb, nib->unk0C);
    sb_write_int(sb, nib->unk10);
    sb_write_int(sb, nib->unk14);
    sb_write_int(sb, nib->unk18);
    sb_write_int(sb, nib->unk1C);
}

NodeItemB read_node_item_b() {
	NodeItemB nib = br_read(&br, NodeItemB);
    assert(nib.unk0C == 0);
    assert(nib.unk10 == 0);
    assert(nib.unk14 == 0);
    assert(nib.unk18 == 0);
    assert(nib.unk1C == 0);
	return nib;
}

typedef struct {
    Varint nodeItemsBOffset;
    short nodeItemBCount;
    short unk0A; //Assert(unk0A == 0);
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
#ifdef VARINT_LONG
        int unk14;
#endif
} NodeListBHeader;

typedef struct {
	NodeListBHeader header;
	DANodeItemB nodeItemsB;
} NodeListB;

void node_list_b_free(NodeListB *nlb) {
	da_foreach(NodeItemB, nib, &nlb->nodeItemsB) {
		node_item_b_free(nib);
	}
	da_free(nlb->nodeItemsB);
}

void write_node_list_b(String_Builder *sb, NodeListB *nlb, size_t i) {
	*ht_put(&writer.nodeItemsBOffsets, i) = sb->count;
    sb_write_Varint(sb, -1);
    sb_write_short(sb, nlb->nodeItemsB.count);
    sb_write_short(sb, nlb->header.unk0A);
    sb_write_int(sb, nlb->header.unk0C);
    sb_write_int(sb, nlb->header.unk10);
#ifdef VARINT_LONG
	sb_write_int(sb, nlb->header.unk14);
#endif
}

NodeListB read_node_list_b() {
	NodeListB nlb = {0};
	nlb.header = br_read(&br, NodeListBHeader);
    assert(nlb.header.unk0A == 0);
    assert(nlb.header.unk0C == 0);
    assert(nlb.header.unk10 == 0);
    size_t pos = br.position;
    FSeek(nlb.header.nodeItemsBOffset);
	da_read(&nlb.nodeItemsB, nlb.header.nodeItemBCount, read_node_item_b);
    FSeek(pos);
	return nlb;
}


typedef struct {
    int unk00;
    short unk04;
    short unk06; //Assert(unk06 == 0);
    int unk08; //Assert(unk08 == 0);
    int unk0C; //Assert(unk0C == 0);
} NodeItemA;

DA(NodeItemA, DANodeItemA);

// NOT NECESSARY
void node_item_a_free(NodeItemA *nia) {
	UNUSED(nia);
}


void write_node_item_a(String_Builder *sb, NodeItemA *nia) {
    sb_write_int(sb, nia->unk00);
    sb_write_short(sb, nia->unk04);
    sb_write_short(sb, nia->unk06);
    sb_write_int(sb, nia->unk08);
    sb_write_int(sb, nia->unk0C);
}


NodeItemA read_node_item_a() {
	NodeItemA nia = br_read(&br, NodeItemA);
    assert(nia.unk06 == 0);
    assert(nia.unk08 == 0);
    assert(nia.unk0C == 0);
	return nia;
}

typedef struct {
    Varint nodeItemsAOffset;
    short nodeItemACount;
    short unk0A; //Assert(unk0A == 0);
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
#ifdef VARINT_LONG
        int unk14;
#endif
} NodeListAHeader;

typedef struct {
	NodeListAHeader header;
	DANodeItemA nodeItemsA;
} NodeListA;

void node_list_a_free(NodeListA *nla) {
	da_foreach(NodeItemA, nia, &nla->nodeItemsA) {
		node_item_a_free(nia);
	}
	da_free(nla->nodeItemsA);
}

void write_node_list_a(String_Builder *sb, NodeListA *nla, size_t i) {
	*ht_put(&writer.nodeItemsAOffsets, i) = sb->count;
    sb_write_Varint(sb, -1);
    sb_write_short(sb, nla->nodeItemsA.count);
    sb_write_short(sb, nla->header.unk0A);
    sb_write_int(sb, nla->header.unk0C);
    sb_write_int(sb, nla->header.unk10);
#ifdef VARINT_LONG
	sb_write_int(sb, nla->header.unk14);
#endif
}

NodeListA read_node_list_a() {
	NodeListA nla = {0};
	nla.header = br_read(&br, NodeListAHeader);
    assert(nla.header.unk0A == 0);
    assert(nla.header.unk0C == 0);
    assert(nla.header.unk10 == 0);
	size_t pos = br.position;
	FSeek(nla.header.nodeItemsAOffset);
	da_read(&nla.nodeItemsA, nla.header.nodeItemACount, read_node_item_a);
	FSeek(pos);
	return nla;
}

typedef struct {
    V2f translation;
    V2f scale;
    V2f skew;
    ubyte color[4]; // RGBA
    int unk1C; // Usually 0, possibly another color?
    int unk20;
    int unk24;
    int unk28; //Assert(unk28 == 0);
    int unk2C; //Assert(unk2C == 0);
} Frame;

DA(Frame, DAFrame);

// NOT NECESSARY
void frame_free(Frame *f) {
	UNUSED(f);
}

void write_frame(String_Builder *sb, const Frame *f) {
    sb_write_V2f(sb, f->translation);
    sb_write_V2f(sb, f->scale);
    sb_write_V2f(sb, f->skew);
    sb_write_ubyte(sb, f->color[0]);
    sb_write_ubyte(sb, f->color[1]);
    sb_write_ubyte(sb, f->color[2]);
    sb_write_ubyte(sb, f->color[3]);
    sb_write_int(sb, f->unk1C);
    sb_write_int(sb, f->unk20);
    sb_write_int(sb, f->unk24);
    sb_write_int(sb, f->unk28);
    sb_write_int(sb, f->unk2C);
}

Frame read_frame() {
	Frame f = br_read(&br, Frame);
    assert(f.unk28 == 0);
    assert(f.unk2C == 0);
	return f;
}

typedef struct {
    short elementID;
    short frameCount;
#ifdef VARINT_LONG
        int pad04; //Assert(pad04 == 0);
#endif
    Varint framesOffset;
    short unk10;
    short unk12;
    short unk14;
    short unk16;
    int unk18;
    int unk1C;
    int unk20; //Assert(unk20 == 0);
    int unk24; //Assert(unk24 == 0);
} AnimatorHeader;

typedef struct {
	AnimatorHeader header;
	DAFrame frames;
} Animator;

DA(Animator, DAAnimator);

void animator_free(Animator *a) {
	da_foreach(Frame, f, &a->frames) {
		frame_free(f);
	}
	da_free(a->frames);
}

void write_animator(String_Builder *sb, const Animator *a, size_t i) {
    sb_write_short(sb, a->header.elementID);
    sb_write_short(sb, a->frames.count);
#ifdef VARINT_LONG
	sb_write_int(sb, a->header.pad04);
#endif
	*ht_put(&writer.framesOffsets, i) = sb->count;
    sb_write_Varint(sb, -1);
    sb_write_short(sb, a->header.unk10);
    sb_write_short(sb, a->header.unk12);
    sb_write_short(sb, a->header.unk14);
    sb_write_short(sb, a->header.unk16);
    sb_write_int(sb, a->header.unk18);
    sb_write_int(sb, a->header.unk1C);
    sb_write_int(sb, a->header.unk20);
    sb_write_int(sb, a->header.unk24);
}

typedef struct {
    short id;
    short animatorCount;
#ifdef VARINT_LONG
        int unk04;
#endif
    Varint animatorsOffset;
    int unk10;
    short unk14; //Assert(unk14 == 1);
    short unk16; //Assert(unk16 == 0);
    Varint nodeListAOffset;
    Varint nodeListBOffset;
    
    // Dubious
    Varint unk28; //Assert(unk28.val == 0);
    Varint unk2C; //Assert(unk2C.val == 0);
    int unk30; //Assert(unk30 == 0);
    int unk34; //Assert(unk34 == 0);
    int unk38; //Assert(unk38 == 0);
    int unk3C; //Assert(unk3C == 0);
} NodeHeader;

typedef struct {
	NodeHeader header;
	DArrz animator_idx;
	NodeListA nodeListA;
	NodeListB nodeListB;
} Node;

DA(Node, DANode);

void node_free(Node *n) {
	da_free(n->animator_idx);
	node_list_a_free(&n->nodeListA);
	node_list_b_free(&n->nodeListB);
}

void write_node(String_Builder *sb, const Node *n, size_t i) {
    sb_write_uint16t(sb, n->header.id);
    sb_write_uint16t(sb, n->animator_idx.count);
#ifdef VARINT_LONG
	sb_write_int32t(sb, n->header.unk04);
#endif
	*ht_put(&writer.animatorOffsets, i) = sb->count;
    sb_write_varint(sb, -1);
    sb_write_int32t(sb, n->header.unk10);
    sb_write_uint16t(sb, n->header.unk14);
    sb_write_uint16t(sb, n->header.unk16);
	*ht_put(&writer.nodeListAOffsets, i) = sb->count;
    sb_write_varint(sb, -1);
	*ht_put(&writer.nodeListBOffsets, i) = sb->count;
    sb_write_varint(sb, -1);
    sb_write_varint(sb, n->header.unk28);
    sb_write_varint(sb, n->header.unk2C);
    sb_write_int32t(sb, n->header.unk30);
    sb_write_int32t(sb, n->header.unk34);
    sb_write_int32t(sb, n->header.unk38);
    sb_write_int32t(sb, n->header.unk3C);
}

typedef struct {
    int id;
    int nodeID;
    Varint nodeOffset;
    V2f position;
    short unk18;
    short unk1A; // byte byte?
    int unk1C; // In SotfS these two are 0
    int unk20; // In DX9 this looks like SotfS's unk24
#ifdef VARINT_LONG
        int unk24;
#endif
} RootHeader;

typedef struct {
	RootHeader header;
	size_t node_idx;
} Root;

DA(Root, DARoot);

// NOT NECESSARY
void root_free(Root *r) {
	UNUSED(r);
}

void write_root(String_Builder *sb, const Root *r) {
    sb_write_int(sb, r->header.id);
    sb_write_int(sb, r->header.nodeID);
    sb_write_Varint(sb, *ht_find(&writer.node_id_offsets, r->header.nodeID));
    sb_write_V2f(sb, r->header.position);
    sb_write_short(sb, r->header.unk18);
    sb_write_short(sb, r->header.unk1A);
    sb_write_int(sb, r->header.unk1C);
    sb_write_int(sb, r->header.unk20);
#ifdef VARINT_LONG
	sb_write_int(sb, r->header.unk24);
#endif
}

typedef char* StringWrapper;

DA(StringWrapper, DAStringWrapper);

StringWrapper read_string() {
	return (StringWrapper)(br.data + br.position);
}

typedef struct {
    short id;
    short unk02; //Assert(unk02 == 0);
    short font;
    short unk06;
    short size;
    short unk0A; //Assert(unk0A == 0);
    short unk0C;
    short unk0E; //Assert(unk0E == 0);
    short unk10;
    ubyte color[4]; // RGBA
    byte unk16;
    byte unk17;
    // Dubious
    Varint unk18; //Assert(unk18.val == 0);
    int unk20; //Assert(unk20 == 0);
    int unk24; //Assert(unk24 == 0);
    int unk28; //Assert(unk28 == 0);
    int unk2C; //Assert(unk2C == 0);
    int unk30; //Assert(unk30 == 0);
#ifdef VARINT_LONG
	int unk34;
#endif
} Text;

DA(Text, DAText);

Text read_text() {
	Text t = br_read(&br, Text);
	return t;
}

void write_text(String_Builder *sb, const Text *t) {
    sb_write_short(sb, t->id);
    sb_write_short(sb, t->unk02);
    sb_write_short(sb, t->font);
    sb_write_short(sb, t->unk06);
    sb_write_short(sb, t->size);
    sb_write_short(sb, t->unk0A);
    sb_write_short(sb, t->unk0C);
    sb_write_short(sb, t->unk0E);
    sb_write_short(sb, t->unk10);
    sb_write_ubyte(sb, t->color[0]);
    sb_write_ubyte(sb, t->color[1]);
    sb_write_ubyte(sb, t->color[2]);
    sb_write_ubyte(sb, t->color[3]);
    sb_write_byte(sb, t->unk16);
    sb_write_byte(sb, t->unk17);
    sb_write_Varint(sb, t->unk18);
    sb_write_int(sb, t->unk20);
    sb_write_int(sb, t->unk24);
    sb_write_int(sb, t->unk28);
    sb_write_int(sb, t->unk2C);
    sb_write_int(sb, t->unk30);
#ifdef VARINT_LONG
	sb_write_int(sb, t->unk34);
#endif
}


Header read_header() {
	Header header = br_read(&br, Header);
	Assert(header.unk00 == 0x20000);
	Assert(header.count2 == 0);
	Assert(header.bigEndian == 0 || header.bigEndian == 1);
	Assert(header.unk59 == 0);
	Assert(header.unk5A == 30 || header.unk5A == 31 || header.unk5A == 60); // Framerate?
	Assert(header.unk5B == 0);
	Assert(header.unk5C == 0);
	Assert(header.unk60 == 0);
	Assert(header.unk64 == 0);
	Assert(header.unk68 == 0);
	Assert(header.unk6C == 0);
	Assert(header.unk70 == 0);
	Assert(header.unk74 == 0);
	Assert(header.unk78 == 0);
	Assert(header.unk7C == 0);
	Assert(header.unk80 == 0);
	Assert(header.unk84 == 0);
	Assert(header.unk88 == 0);
	Assert(header.unk8C == 0);
	Assert(header.unk90 == 0);
	Assert(header.unk94 == 0);
	Assert(header.unk98 == 0);
	Assert(header.unk9C == 0);
	Assert(header.unkA0 == 0);
	return header;
}

DA(SpriteSet, DASpriteSet);

typedef struct {
	Header header;
	DASpriteSet spriteSets;
	DANode nodes;
	DAAnimator animators;
	DARoot roots;
	DATexture textures;
	DAStringWrapper strings;
	DAText texts;

	Dictzuzu texture_offsets;
	Dictzuzu node_offsets;
	Dictzuzu animator_offsets;
} FLO;

void flo_free(FLO *flo) {
	da_foreach(SpriteSet, ss, &flo->spriteSets) {
		sprite_set_free(ss);
	}
	da_free(flo->spriteSets);
	da_foreach(Node, n, &flo->nodes) {
		node_free(n);
	}
	da_free(flo->nodes);
	da_foreach(Animator, a, &flo->animators) {
		animator_free(a);
	}
	da_free(flo->animators);
	da_foreach(Root, r, &flo->roots) {
		root_free(r);
	}
	da_free(flo->roots);
	da_foreach(Texture, t, &flo->textures) {
		texture_free(t);
	}
	da_free(flo->textures);
	da_free(flo->strings);
	ht_free(&flo->texture_offsets);
	ht_free(&flo->node_offsets);
	ht_free(&flo->animator_offsets);
}

size_t read_texture(FLO *flo) {
	size_t *index = ht_find(&flo->texture_offsets, br.position);
	if (index != NULL) {
		return *index;
	}
	size_t new_idx = flo->textures.count;
	*ht_put(&flo->texture_offsets, br.position) = new_idx;
	Texture t = {0};
	t.texture = br_read(&br, TextureHeader);
    assert(t.texture.unk0A == 0);
    assert(t.texture.unk0C == 0);
    assert(t.texture.unk12 == 0);
	t.name = (const char *)(br.data + t.texture.name_offset);
	da_append(&flo->textures, t);
	return new_idx;
}

Sprite read_sprite(FLO *flo) {
	Sprite sprite = {0};
	sprite.header = br_read(&br, SpriteHeader);
    assert(sprite.header.unk18 == -1);
#ifdef VARINT_LONG
	assert(sprite.header.pad1C == 0);
#endif
    assert(sprite.header.unk2A == 0);
#ifdef VARINT_LONG
	assert(sprite.header.pad2C == 0);
#endif
    assert(sprite.header.unk38 == 0);
    assert(sprite.header.unk3C == 0);
	size_t pos = br.position;
	sprite.texture_idx = *ht_find(&flo->texture_offsets, sprite.header.textureOffset);
    FSeek(sprite.header.texRegionOffset);
    sprite.texRegion = read_tex_region();
    FSeek(pos);
	return sprite;
}

SpriteSet read_sprite_set(FLO *flo) {
	SpriteSet ss = {0};
	ss.header = br_read(&br, SpriteSetHeader);
#ifdef VARINT_LONG
	assert(ss.header.pad04 == 0);
#endif
	assert(ss.header.unk10 == 0);
	assert(ss.header.unk14 == 0);
	size_t pos = br.position;
	FSeek(ss.header.spritesOffset);
	da_read_arg(&ss.sprites, ss.header.spriteCount, read_sprite, flo);
	FSeek(pos);
	return ss;
}

size_t read_animator(FLO *flo) {
	size_t *index = ht_find(&flo->animator_offsets, br.position);
	if (index != NULL) {
		return *index;
	}
	size_t new_idx = flo->animators.count;
	*ht_put(&flo->animator_offsets, br.position) = new_idx;
	Animator a = {0};
	a.header = br_read(&br, AnimatorHeader);
#ifdef VARINT_LONG
	assert(a.header.pad04 == 0);
#endif
    assert(a.header.unk20 == 0);
    assert(a.header.unk24 == 0);
	size_t pos = br.position;
	FSeek(a.header.framesOffset);
	da_read(&a.frames, a.header.frameCount, read_frame);
	FSeek(pos);
	da_append(&flo->animators, a);
	return new_idx;
}

size_t read_node(FLO *flo) {
	size_t *index = ht_find(&flo->node_offsets, br.position);
	if (index != NULL) {
		return *index;
	}
	size_t new_idx = flo->nodes.count;
	*ht_put(&flo->node_offsets, br.position) = new_idx;
	Node node = {0};
	node.header = br_read(&br, NodeHeader);
    assert(node.header.unk14 == 1);
    assert(node.header.unk16 == 0);
    assert(node.header.unk28 == 0);
    assert(node.header.unk2C == 0);
    assert(node.header.unk30 == 0);
    assert(node.header.unk34 == 0);
    assert(node.header.unk38 == 0);
    assert(node.header.unk3C == 0);
	size_t pos = br.position;
    FSeek(node.header.animatorsOffset);
	da_read_arg(&node.animator_idx, node.header.animatorCount, read_animator, flo);
	if (node.header.nodeListAOffset != 0) {
        FSeek(node.header.nodeListAOffset);
		node.nodeListA = read_node_list_a();
	}
	if (node.header.nodeListBOffset != 0) {
        FSeek(node.header.nodeListBOffset);
		node.nodeListB = read_node_list_b();
	}
    FSeek(pos);
	da_append(&flo->nodes, node);
	return new_idx;
}

Root read_root(FLO *flo) {
	Root root = {0};
	root.header = br_read(&br, RootHeader);
    size_t pos = br.position;
    FSeek(root.header.nodeOffset);
    root.node_idx = read_node(flo);
    FSeek(pos);
	return root;
}

FLO read_flo(String_Builder sb) {
	br = sb_to_br(sb);
	FLO flo = {0};
	flo.header = read_header();
	if (flo.header.stringCount > 0) {
		FSeek(flo.header.stringsOffset);
		da_read(&flo.strings, flo.header.stringCount, read_string);
	}
	if (flo.header.textureCount > 0) {
		FSeek(flo.header.texturesOffset);
		da_read_null(&flo.textures, flo.header.textureCount, read_texture, &flo);
	}
	if (flo.header.animatorCount > 0) {
		FSeek(flo.header.animatorsOffset);
		da_read_null(&flo.animators, flo.header.animatorCount, read_animator, &flo);
	}
	if (flo.header.nodeCount > 0) {
		FSeek(flo.header.nodesOffset);
		da_read_null(&flo.nodes, flo.header.nodeCount, read_node, &flo);
	}
	if (flo.header.rootCount > 0) {
		FSeek(flo.header.rootsOffset);
		da_read_arg(&flo.roots, flo.header.rootCount, read_root, &flo);
	}
	if (flo.header.spriteSetCount > 0) {
		FSeek(flo.header.spriteSetsOffset);
		da_read_arg(&flo.spriteSets, flo.header.spriteSetCount, read_sprite_set, &flo);
	}
	if (flo.header.textCount > 0) {
		FSeek(flo.header.textsOffset);
		da_read(&flo.texts, flo.header.textCount, read_text);
	}
	return flo;
}

void write_header(String_Builder *sb, const FLO *flo) {
    sb_write_int32t(sb, flo->header.unk00);
#ifdef VARINT_LONG
	sb_write_int32t(sb, flo->header.unk04);
#endif
	writer.spriteSetsOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.offset2 = sb->count;
    sb_write_varint(sb, -1);
	writer.nodesOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.animatorsOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.rootsOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.texturesOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.stringsOffset = sb->count;
    sb_write_varint(sb, -1);
	writer.textsOffset = sb->count;
    sb_write_varint(sb, -1);
    sb_write_int16t(sb, flo->spriteSets.count);
    sb_write_int16t(sb, flo->header.count2);
    sb_write_int16t(sb, flo->nodes.count);
    sb_write_int16t(sb, flo->animators.count);
    sb_write_int16t(sb, flo->roots.count);
    sb_write_int16t(sb, flo->textures.count);
    sb_write_int16t(sb, flo->strings.count);
    sb_write_int16t(sb, flo->texts.count);
    sb_append(sb, flo->header.bigEndian);
    sb_append(sb, flo->header.unk59);
    sb_append(sb, flo->header.unk5A);
    sb_append(sb, flo->header.unk5B);
    sb_write_int32t(sb, flo->header.unk5C);
    sb_write_int32t(sb, flo->header.unk60);
    sb_write_int32t(sb, flo->header.unk64);
    sb_write_int32t(sb, flo->header.unk68);
    sb_write_int32t(sb, flo->header.unk6C);
    sb_write_int32t(sb, flo->header.unk70);
    sb_write_int32t(sb, flo->header.unk74);
    sb_write_int32t(sb, flo->header.unk78);
    sb_write_int32t(sb, flo->header.unk7C);
    sb_write_int32t(sb, flo->header.unk80);
    sb_write_int32t(sb, flo->header.unk84);
    sb_write_int32t(sb, flo->header.unk88);
    sb_write_int32t(sb, flo->header.unk8C);
    sb_write_int32t(sb, flo->header.unk90);
    sb_write_int32t(sb, flo->header.unk94);
    sb_write_int32t(sb, flo->header.unk98);
    sb_write_int32t(sb, flo->header.unk9C);
    sb_write_int32t(sb, flo->header.unkA0);
#ifdef VARINT_LONG
    sb_write_int32t(sb, flo->header.unkA4);
#endif
}

void write_flo(String_Builder *sb, const FLO *flo) {
	writer = (FLO_Writer){0};
	write_header(sb, flo);
	sb_patch_offset(sb, writer.spriteSetsOffset);
	// sprite sets
	for (size_t i=0; i<flo->spriteSets.count; i++) {
		SpriteSet *ss = &flo->spriteSets.items[i];
		write_sprite_set(sb, ss, i);
		sb_pad_align(sb, 8);
	}
	// sprites
	for (size_t i=0; i<flo->spriteSets.count; i++) {
		SpriteSet *ss = &flo->spriteSets.items[i];
		sb_patch_offset(sb, *ht_find(&writer.spriteSet_offsets, i));
		for (size_t j=0; j<ss->sprites.count; j++) {
			Sprite *s = &ss->sprites.items[j];
			write_sprite(sb, s, (Pair){i, j});
		}
	}
	// tex region
	for (size_t i=0; i<flo->spriteSets.count; i++) {
		SpriteSet *ss = &flo->spriteSets.items[i];
		for (size_t j=0; j<ss->sprites.count; j++) {
			Sprite *s = &ss->sprites.items[j];
			sb_patch_offset(sb, *ht_find(&writer.texRegionOffsetsOffset, ((Pair){i, j})));
			write_texregion(sb, &s->texRegion);
		}
	}
	// nodes
	sb_patch_offset(sb, writer.nodesOffset);
	sb_patch_offset(sb, writer.offset2); // same as nodesOffset??
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		assert(ht_find(&writer.node_id_offsets, n->header.id) == NULL && "Same node id detected");
		*ht_put(&writer.node_id_offsets, n->header.id) = sb->count;
		write_node(sb, n, i);
	}
	// node list b
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		NodeListB *nlb = &n->nodeListB;
		if (nlb->nodeItemsB.count > 0) sb_patch_offset(sb, *ht_find(&writer.nodeListBOffsets, i));
		else sb_patch_offset_value(sb, *ht_find(&writer.nodeListBOffsets, i), 0);
		if (nlb->nodeItemsB.count > 0) {
			write_node_list_b(sb, nlb, i);
		}
	}
	// node items b
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		NodeListB *nlb = &n->nodeListB;
		if (nlb->nodeItemsB.count > 0) sb_patch_offset(sb, *ht_find(&writer.nodeItemsBOffsets, i));
		for (size_t j=0; j<nlb->nodeItemsB.count; j++) {
			NodeItemB *nib = &nlb->nodeItemsB.items[j];
			write_node_item_b(sb, nib);
		}
	}
	// node list a
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		NodeListA *nla = &n->nodeListA;
		if (nla->nodeItemsA.count > 0) sb_patch_offset(sb, *ht_find(&writer.nodeListAOffsets, i));
		else sb_patch_offset_value(sb, *ht_find(&writer.nodeListAOffsets, i), 0);
		if (nla->nodeItemsA.count > 0) {
			write_node_list_a(sb, nla, i);
		}
	}
	// node items a
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		NodeListA *nla = &n->nodeListA;
		if (nla->nodeItemsA.count > 0) sb_patch_offset(sb, *ht_find(&writer.nodeItemsAOffsets, i));
		for (size_t j=0; j<nla->nodeItemsA.count; j++) {
			NodeItemA *nia = &nla->nodeItemsA.items[j];
			write_node_item_a(sb, nia);
		}
	}
	// animators
	sb_patch_offset(sb, writer.animatorsOffset);
	for (size_t i=0; i<flo->animators.count; i++) {
		Animator *a = &flo->animators.items[i];
		*ht_put(&writer.animatorOffsetOffsets, i) = sb->count;
		write_animator(sb, a, i);
	}
	// backpatch animator offsets in nodes
	for (size_t i=0; i<flo->nodes.count; i++) {
		Node *n = &flo->nodes.items[i];
		size_t *patch_dst = ht_find(&writer.animatorOffsets, i);
		if (n->animator_idx.count > 0) {
			size_t animator_id = n->animator_idx.items[0];
			size_t *patch_src = ht_find(&writer.animatorOffsetOffsets, animator_id);
			assert(patch_dst != NULL && patch_src != NULL);
			sb_patch_offset_value(sb, *patch_dst, *patch_src);
		}
	}
	// frames
	for (size_t i=0; i<flo->animators.count; i++) {
		// to create 1 to 1 copy these swaps are necessay for l03_01_Bonfire file
		// if      (i == 579) i = 580;
		// else if (i == 581) i = 584;
		// else if (i == 585) i = 579;
		// else if (i == 580) i = 582;
		// else if (i == 584) i = 581;
		// else if (i == 582) i = 585;
		// else if (i == 587) i = 588;
		// else if (i == 589) i = 587;
		// else if (i == 588) i = 589;
		// else if (i == 661) i = 662;
		// else if (i == 663) i = 661;
		// else if (i == 662) i = 663;
		// else if (i == 701) i = 702;
		// else if (i == 703) i = 701;
		// else if (i == 702) i = 703;
		Animator *a = &flo->animators.items[i];
		sb_patch_offset(sb, *ht_find(&writer.framesOffsets, i));
		for (size_t j=0; j<a->frames.count; j++) {
			Frame *f = &a->frames.items[j];
			write_frame(sb, f);
		}
	}
	// roots
	sb_patch_offset(sb, writer.rootsOffset);
	for (size_t i=0; i<flo->roots.count; i++) {
		Root *r = &flo->roots.items[i];
		write_root(sb, r);
	}
	// textures
	sb_patch_offset(sb, writer.texturesOffset);
	for (size_t i=0; i<flo->textures.count; i++) {
		Texture *t = &flo->textures.items[i];
		*ht_put(&writer.texture_id_offsets, t->texture.id) = sb->count;
		write_texture(sb, t);
	}
	// texts
	sb_patch_offset(sb, writer.textsOffset);
	for (size_t i=0; i<flo->texts.count; i++) {
		Text *t = &flo->texts.items[i];
		write_text(sb, t);
	}
	// texture names
	sb_patch_offset(sb, writer.stringsOffset);
	for (size_t i=0; i<flo->textures.count; i++) {
		Texture *t = &flo->textures.items[i];
		size_t *offset = ht_find(&writer.texture_names, t->name);
		sb_patch_offset(sb, *offset);
		sb_append_cstr(sb, ht_key(&writer.texture_names, offset));
		sb_append_null(sb);
	}
	// ht_foreach(offset, &writer.texture_names) {
	// 	sb_patch_offset(sb, *offset);
	// 	sb_append_cstr(sb, ht_key(&writer.texture_names, offset));
	// 	sb_append_null(sb);
	// }
	// backpatch textures in sprites
	for (size_t i=0; i<flo->spriteSets.count; i++) {
		SpriteSet *ss = &flo->spriteSets.items[i];
		for (size_t j=0; j<ss->sprites.count; j++) {
			Pair p = *ht_find(&writer.texturesOffsetsOffset, ((Pair){i, j}));
			size_t texture_id = p.from;
			size_t sprite_texture_offset = p.to;
			size_t texture_offset = *ht_find(&writer.texture_id_offsets, texture_id);
			sb_patch_offset_value(sb, sprite_texture_offset, texture_offset);
		}
	}
	sb_pad_align(sb, 0x20);
}

// #define PATH "menu/01-febnd-dcx/l01_01_hp.flo"
#define PATH "menu/03-febnd-dcx/l03_01_Bonfire.flo"

int main() {
	String_Builder sb = {0};
	if (!read_entire_file("./data/"PATH, &sb)) return 1;
	FLO flo = read_flo(sb);

	flo.roots.count = 0;
	flo.spriteSets.count = 0;
	flo.nodes.count = 0;
	flo.animators.count = 0;
	flo.textures.count = 0;
	flo.strings.count = 0;
	flo.texts.count = 0;

	String_Builder wsb = {0};
	write_flo(&wsb, &flo);
	if (!mkdir_if_not_exists("output")) return 1;
	if (!mkdir_if_not_exists("output/menu")) return 1;
	// if (!mkdir_if_not_exists("output/menu/01-febnd-dcx")) return 1;
	if (!mkdir_if_not_exists("output/menu/03-febnd-dcx")) return 1;
	if (!write_entire_file("output/"PATH, wsb.items, wsb.count)) return 1;
	flo_free(&flo);
	da_free(sb);
	da_free(wsb);

	Cmd cmd = {0};
	cmd_append(&cmd, "D:\\utils\\witchy-bnd\\WitchyBND.exe");
	cmd_append(&cmd, "output/menu/03-febnd-dcx");
	if (!cmd_run(&cmd)) return 1;
	return 0;
}
