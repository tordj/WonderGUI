#ifndef WG_TEXTVIEW_DOT_H
#define WG_TEXTVIEW_DOT_H

class WgTextContainer;

class WgTextView
{
friend class WgTextContainer;

public:

	void				SetContainer( WgTextContainer * pContainer );
	inline WgTextContainer *	Container() { return m_pContainer; };

protected:
	WgTextView();
	~WgTextView();

	virtual void		refreshView() = 0;


	WgTextContainer *	m_pContainer;
	WgTextView *		m_pNextView;
};


#endif // WG_TEXTVIEW_DOT_H
