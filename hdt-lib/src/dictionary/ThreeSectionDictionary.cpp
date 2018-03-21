namespace hdt {

ThreeSectionDictionary::ThreeSectionDictionary(): blocksize(16), subjects(new csd::CSD_PFC()), objects(new csd::CSD_PFC()), shared(new csd::CSD_PFC()) {}

ThreeSectionDictionary::ThreeSectionDictionary(HDTSpecification & spec): blocksize(16), subjects(new csd::CSD_PFC()), objects(new csd::CSD_PFC()), shared(new csd::CSD_PFC())
{
	string blockSizeStr = "";
	try{
		spec.get("dict.block.size");
	}catch(exception& e){}

	if(blockSizeStr!=""){
		//blocksize = atoi((const char*)blockSizeStr.c_str());
	}
}

ThreeSectionDictionary::~ThreeSectionDictionary()
{clear();}

void ThreeSectionDictionary::clear()
{
	if (subjects!=NULL)
		{delete subjects; subjects=NULL;}
	if (objects!=NULL)
		{delete objects; objects=NULL;}
	if (shared!=NULL)
		{delete shared; shared=NULL;}
}
void ThreeSectionDictionary::create()
{
	clear();
	subjects = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();
}

csd::CSD *loadSection(IteratorUCharString *iterator, uint32_t blocksize, ProgressListener *listener) {
	return new csd::CSD_PFC(iterator, blocksize, listener);
	//return new csd::CSD_HTFC(iterator, blocksize, listener);
}






std::string ThreeSectionDictionary::idToString(const unsigned int id, const TripleComponentRole position)const
{
	csd::CSD *section = getDictionarySection(id, position);

	unsigned int localid = getLocalId(id, position);

	if(localid<=section->getLength()) {
		const char * ptr = (const char *)section->extract(localid);
		if(ptr!=NULL) {
			string out = ptr;
			section->freeString((unsigned char*)ptr);
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

unsigned int ThreeSectionDictionary::stringToId(const std::string &key, const TripleComponentRole position)const
{
	unsigned int ret;

        if(key.length()==0)
		return 0;
        
	
	if(position==SUBJECT) {
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) 
			return getGlobalId(ret,SHARED_SUBJECT);
		ret = subjects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) 
			return getGlobalId(ret,NOT_SHARED_SUBJECT);
        	return 0;
	}
	else if (position==OBJECT){
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) 
			return getGlobalId(ret,SHARED_OBJECT);
		ret = objects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) 
			return getGlobalId(ret,NOT_SHARED_OBJECT);
        	return 0;
	}
	else
		return 0;
}





void GraphsFourSectionDictionary::load(std::istream & input, ControlInformation & ci, ProgressListener *listener/*=NULL*/)
{
	IntermediateListener iListener(listener);

	loadControlInfo(input, ci);
	loadShared(input, iListener);
	loadSubjects(input, iListener);
	loadFourthSection(input, iListener);
	loadObjects(input, iListener);
}

void ThreeSectionDictionary::loadControlInfo(std::istream & input, ControlInformation & ci)
{
	std::string format = ci.getFormat();
	if(format!=getType()) {
		throw std::runtime_error("Trying to read a ThreeSectionDictionary but the data is not ThreeSectionDictionary");
	}
	//this->mapping = ci.getUint("mapping");
	this->mapping = MAPPING2;
	//this->sizeStrings = ci.getUint("sizeStrings");
}

void ThreeSectionDictionary::loadShared(std::istream & input, IntermediateListener& iListener)
{
	iListener.setRange(0,25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	delete shared;
	shared = csd::CSD::load(input);
	if(shared==NULL){
		shared = new csd::CSD_PFC();
		throw std::runtime_error("Could not read shared.");
	}
	//shared = new csd::CSD_Cache(shared);
}
void ThreeSectionDictionary::loadSubjects(std::istream & input, IntermediateListener& iListener)
{
	iListener.setRange(25,50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	delete subjects;
	subjects = csd::CSD::load(input);
	if(subjects==NULL){
		subjects = new csd::CSD_PFC();
		throw std::runtime_error("Could not read subjects.");
	}
	//subjects = new csd::CSD_Cache(subjects);
}
void ThreeSectionDictionary::loadObjects(std::istream & input, IntermediateListener& iListener)
{
	iListener.setRange(75,100);
	iListener.notifyProgress(0, "Dictionary read objects.");
	delete objects;
	objects = csd::CSD::load(input);
	if(objects==NULL){
		objects = new csd::CSD_PFC();
		throw std::runtime_error("Could not read objects.");
	}
	//objects = new csd::CSD_Cache(objects);
}


size_t GraphsFourSectionDictionary::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener/*=NULL*/)
{
    size_t count=0;
    IntermediateListener iListener(listener);
	
    loadControlInfo(ptr, ptrMax, count);
    loadShared(ptr, ptrMax, count, iListener);
    loadSubjects(ptr, ptrMax, count, iListener);
    loadFourthSection(ptr, ptrMax, count, iListener);
    loadObjects(ptr, ptrMax, count, iListener);

    return count;
}
void ThreeSectionDictionary::loadControlInfo(unsigned char *ptr, unsigned char *ptrMax, size_t& count){
    ControlInformation ci;
    count += ci.load(&ptr[count], ptrMax);

    //this->mapping = ci.getUint("mapping");
    this->mapping = MAPPING2;
    //this->sizeStrings = ci.getUint("sizeStrings");
}

void ThreeSectionDictionary::loadShared(unsigned char *ptr, unsigned char *ptrMax, size_t& count, IntermediateListener& iListener)
{
    iListener.setRange(0,25);
    iListener.notifyProgress(0, "Dictionary read shared area.");
    delete shared;
    shared = csd::CSD::create(ptr[count]);
    if(shared==NULL){
        shared = new csd::CSD_PFC();
        throw std::runtime_error("Could not read shared.");
    }
    count += shared->load(&ptr[count], ptrMax);
    //shared = new csd::CSD_Cache(shared);
}

void ThreeSectionDictionary::loadSubjects(unsigned char *ptr, unsigned char *ptrMax, size_t& count, IntermediateListener& iListener)
{
    iListener.setRange(25,50);
    iListener.notifyProgress(0, "Dictionary read subjects.");
    delete subjects;
    subjects = csd::CSD::create(ptr[count]);
    if(subjects==NULL){
        subjects = new csd::CSD_PFC();
        throw std::runtime_error("Could not read subjects.");
    }
    count += subjects->load(&ptr[count], ptrMax);
    //subjects = new csd::CSD_Cache(subjects);
}
void ThreeSectionDictionary::loadObjects(unsigned char *ptr, unsigned char *ptrMax, size_t& count, IntermediateListener& iListener)
{
    iListener.setRange(75,100);
    iListener.notifyProgress(0, "Dictionary read objects.");
    delete objects;
    objects = csd::CSD::create(ptr[count]);
    if(objects==NULL){
        objects = new csd::CSD_PFC();
        throw std::runtime_error("Could not read objects.");
    }
    count += objects->load(&ptr[count], ptrMax);
    //objects = new csd::CSD_Cache(objects);
    
}



void TriplesFourSectionDictionary::import(Dictionary *other, ProgressListener *listener) 
{
	try 
	{
		IntermediateListener iListener(listener);
		importSubjects(other, listener, iListener);
		importFourthSection(other, listener, iListener);
		importObjects(other, listener, iListener);
		importShared(other, listener, iListener);

		sizeStrings = other->size();
		mapping = other->getMapping();
	}
	catch (std::exception& e) 
	{
		ThreeSectionDictionary::clear();
		clear();
		ThreeSectionDictionary::create();
		create();
		throw;
	}
}	
void ThreeSectionDictionary::importSubjects(Dictionary *other, ProgressListener *listener, IntermediateListener& iListener) 
{
	NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
	iListener.setRange(0, 20);
	IteratorUCharString *itSubj = other->getSubjects();
	delete subjects;
	subjects = loadSection(itSubj, blocksize, &iListener);
	delete itSubj;
}
void ThreeSectionDictionary::importObjects(Dictionary *other, ProgressListener *listener, IntermediateListener& iListener) 
{
	NOTIFY(listener, "DictionaryPFC loading objects", 30, 90);
	iListener.setRange(21, 90);
	IteratorUCharString *itObj = other->getObjects();
	delete objects;
	objects = loadSection(itObj, blocksize, &iListener);
	delete itObj;
}
void ThreeSectionDictionary::importShared(Dictionary *other, ProgressListener *listener, IntermediateListener& iListener) 
{
	NOTIFY(listener, "DictionaryPFC loading shared", 90, 100);
	iListener.setRange(90, 100);
	IteratorUCharString *itShared = other->getShared();
	delete shared;
	shared = loadSection(itShared, blocksize, &iListener);
	delete itShared;
}

IteratorUCharString *ThreeSectionDictionary::getSubjects()const {
	return subjects->listAll();
}
IteratorUCharString *ThreeSectionDictionary::getObjects()const {
	return objects->listAll();
}
IteratorUCharString *ThreeSectionDictionary::getShared()const {
	return shared->listAll();
}



void TriplesFourSectionDictionary::save(std::ostream& output, ControlInformation & controlInformation, ProgressListener *listener){

	IntermediateListener iListener(listener);

	saveControlInfo(output, controlInformation);
	saveShared(output, iListener);
	saveSubjects(output, iListener);
	saveFourthSection(output, iListener);	
	saveObjects(output, iListener);	
}

void ThreeSectionDictionary::saveControlInfo(std::ostream& output, ControlInformation & controlInformation)
{
	controlInformation.setFormat(HDTVocabulary::DICTIONARY_TYPE_FOUR);
	controlInformation.setUint("mapping", this->mapping);
	controlInformation.setUint("sizeStrings", this->sizeStrings);
	controlInformation.save(output);
}

void ThreeSectionDictionary::saveShared(std::ostream& output, IntermediateListener& iListener)
{
	iListener.setRange(0,10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	shared->save(output);
}
void ThreeSectionDictionary::saveSubjects(std::ostream& output, IntermediateListener& iListener)
{
	iListener.setRange(10,45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	subjects->save(output);
}
void ThreeSectionDictionary::saveObjects(std::ostream& output, IntermediateListener& iListener)
{
	iListener.setRange(60,100);
	iListener.notifyProgress(0, "Dictionary save objects.");
	objects->save(output);
}


void ThreeSectionDictionary::populateHeader(Header& header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE, getType());
#if 0
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS, getNsubjects());
	//header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES, getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS, getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID, getMaxSubjectID());
	//header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID, getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID, getMaxObjectID());
#endif
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getNshared());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE, this->blocksize);
}

unsigned int ThreeSectionDictionary::getNsubjects()const
{return shared->getLength()+subjects->getLength();}
unsigned int ThreeSectionDictionary::getNobjects()const
{return shared->getLength()+objects->getLength();}
unsigned int ThreeSectionDictionary::getNshared()const
{return shared->getLength();}

unsigned int ThreeSectionDictionary::getMaxID()const
{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();
	unsigned int max = s>o ? s : o;

	return (mapping ==MAPPING2) ? sh+max : sh+s+o;
}
unsigned int ThreeSectionDictionary::getMaxSubjectID()const
{return getNsubjects();}

unsigned int ThreeSectionDictionary::getMaxObjectID()const{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();

	return (mapping == MAPPING2) ? sh+o : sh+s+o;
}
size_t ThreeSectionDictionary::getNumberOfElements()const{
	return shared->getLength()+subjects->getLength()+objects->getLength();
}
uint64_t ThreeSectionDictionary::size()const{
	return shared->getSize()+subjects->getSize()+objects->getSize();
}

string FourSectionDictionary::getType()const
{return HDTVocabulary::DICTIONARY_TYPE_FOUR;}

unsigned int FourSectionDictionary::getMapping()const
{return mapping;}




csd::CSD *ThreeSectionDictionary::getDictionarySection(unsigned int id, TripleComponentRole position) const{
	if (position == SUBJECT) 
		return (id<=shared->getLength()) ? shared : subjects;
	else if (position == OBJECT)
		return (id<=shared->getLength()) ? shared : objects;

	throw std::runtime_error("Item not found");
}

unsigned int ThreeSectionDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) const{
	switch (position) {
	case NOT_SHARED_SUBJECT:
		return shared->getLength()+id;
	case NOT_SHARED_OBJECT:
		return (mapping==MAPPING2) ? shared->getLength()+id : shared->getLength()+subjects->getLength()+id;
	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		return id;
	}

	throw std::runtime_error("Item not found");
}

unsigned int ThreeSectionDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) const{
	switch (position) {
	case SUBJECT:
		return (id<=shared->getLength()) ? id : id-shared->getLength();
		break;
	case OBJECT:
		if(id<=shared->getLength()) 
			return id;
		else 
			return (mapping==MAPPING2) ? id-shared->getLength() : 2+id-shared->getLength()-subjects->getLength();
		break;
	}

	throw std::runtime_error("Item not found");
}

void ThreeSectionDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{

	vector<string> v1,v2;
	shared->fillSuggestions(base, v1, maxResults);
	if(role==SUBJECT) {
		subjects->fillSuggestions(base, v2, maxResults);
	} else if(role==OBJECT){
		objects->fillSuggestions(base, v2, maxResults);
	}

	// Merge results from shared and subjects/objects keeping order
	merge(v1.begin(),v1.end(), v2.begin(), v2.end(), std::back_inserter(out));

	// Remove possible extra items
	if(out.size()>maxResults) {
		out.resize(maxResults);
	}
}


}
