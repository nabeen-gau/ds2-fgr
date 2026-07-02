#include "common.c"

BinaryReader br = {0};

#define Varint int64_t
#define Assert assert
#define byte uint8_t
#define VARINT_LONG
#define ubyte byte

#define FSeek(offs) br.position = (offs)

#define da_read(da, n, f) \
	do {\
		da_init((da), (n));\
		for (size_t i=0; i<(size_t)(n); i++) {\
			da_append((da), f());\
		}\
	} while (0)

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

DA(Texture, DATexture);

Texture read_texture() {
	Texture t = {0};
	t.texture = br_read(&br, TextureHeader);
    assert(t.texture.unk0A == 0);
    assert(t.texture.unk0C == 0);
    assert(t.texture.unk12 == 0);
	t.name = (const char *)(br.data + t.texture.name_offset);
	return t;
}

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
	Texture texture;
    TexRegion texRegion;
} Sprite;

void sprite_free(Sprite *s) {
	texture_free(&s->texture);
	tex_region_free(&s->texRegion);
}

Sprite read_sprite() {
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
    FSeek(sprite.header.textureOffset);
	sprite.texture = read_texture();
    FSeek(sprite.header.texRegionOffset);
    sprite.texRegion = read_tex_region();
    FSeek(pos);
	return sprite;
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

SpriteSet read_sprite_set() {
	SpriteSet ss = {0};
	ss.header = br_read(&br, SpriteSetHeader);
#ifdef VARINT_LONG
	assert(ss.header.pad04 == 0);
#endif
	assert(ss.header.unk10 == 0);
	assert(ss.header.unk14 == 0);
	size_t pos = br.position;
	FSeek(ss.header.spritesOffset);
	da_read(&ss.sprites, ss.header.spriteCount, read_sprite);
	FSeek(pos);
	return ss;
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

Animator read_animator() {
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
	return a;
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
	DAAnimator animators;
	NodeListA nodeListA;
	NodeListB nodeListB;
} Node;

DA(Node, DANode);

void node_free(Node *n) {
	da_foreach(Animator, a, &n->animators) {
		animator_free(a);
	}
	da_free(n->animators);
	node_list_a_free(&n->nodeListA);
	node_list_b_free(&n->nodeListB);
}

Node read_node() {
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
	da_read(&node.animators, node.header.animatorCount, read_animator);
	if (node.header.nodeListAOffset != 0) {
        FSeek(node.header.nodeListAOffset);
		node.nodeListA = read_node_list_a();
	}
	if (node.header.nodeListBOffset != 0) {
        FSeek(node.header.nodeListBOffset);
		node.nodeListB = read_node_list_b();
	}
    FSeek(pos);
	return node;
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
	Node node;
} Root;

DA(Root, DARoot);

void root_free(Root *r) {
	node_free(&r->node);
}

Root read_root() {
	Root root = {0};
	root.header = br_read(&br, RootHeader);
    size_t pos = br.position;
    FSeek(root.header.nodeOffset);
    root.node = read_node();
    FSeek(pos);
	return root;
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
}

int main() {
	String_Builder sb = {0};
	if (!read_entire_file("./data/menu/01-febnd-dcx/l01_01_hp.flo", &sb)) return 1;

	br = sb_to_br(sb);
	FLO flo = {0};
	flo.header = read_header();
	if (flo.header.spriteSetCount > 0) {
		FSeek(flo.header.spriteSetsOffset);
		da_read(&flo.spriteSets, flo.header.spriteSetCount, read_sprite_set);
	}
	if (flo.header.nodeCount > 0) {
		FSeek(flo.header.nodesOffset);
		da_read(&flo.nodes, flo.header.nodeCount, read_node);
	}
	if (flo.header.animatorCount > 0) {
		FSeek(flo.header.animatorsOffset);
		da_read(&flo.animators, flo.header.animatorCount, read_animator);
	}
	if (flo.header.rootCount > 0) {
		FSeek(flo.header.rootsOffset);
		da_read(&flo.roots, flo.header.rootCount, read_root);
	}
	if (flo.header.textureCount > 0) {
		FSeek(flo.header.texturesOffset);
		da_read(&flo.textures, flo.header.textureCount, read_texture);
	}
	if (flo.header.stringCount > 0) {
		FSeek(flo.header.stringsOffset);
		da_read(&flo.strings, flo.header.stringCount, read_string);
	}

	flo_free(&flo);
}
