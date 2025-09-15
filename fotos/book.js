let booksCollection;

function injectDB(db)
{
  booksCollection = db.collection('books');
}

async function getAllBooks(req, res)
{
  try
  {
    const books = await booksCollection.find({}).toArray();
    res.json(books);
  }
  catch (err)
  {
    res.status(500).json({ error: err.message });
  }
}

async function addBook(req, res)
{
  try
  {
    const book = req.body;
    book.createdAt = new Date();
    book.updatedAt = new Date();
    const result = await booksCollection.insertOne(book);
    res.status(201).json({ insertedId: result.insertedId });
  }
  catch (err)
  {
    res.status(500).json({ error: err.message });
  }
}

module.exports = {
  injectDB,
  getAllBooks,
  addBook,
};