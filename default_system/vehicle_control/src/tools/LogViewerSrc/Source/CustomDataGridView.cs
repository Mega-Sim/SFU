using System;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace WinForms
{
    /// <summary>
    /// Customised row collection to suppress collection change events
    /// </summary>
    public class CustomDataGridViewRowCollection : DataGridViewRowCollection, ICollection
    {
        public CustomDataGridViewRowCollection(CustomDataGridView grid)
            : base(grid)
        {
        }

        readonly static CollectionChangeEventArgs _RefreshCollectionArgs = new CollectionChangeEventArgs(CollectionChangeAction.Refresh, null);
        protected override void OnCollectionChanged(CollectionChangeEventArgs e)
        {
            //Suppress
            ((CustomDataGridView)base.DataGridView).OnRowCollectionDirty();
        }

        internal void Refresh()
        {
            base.OnCollectionChanged(_RefreshCollectionArgs);
        }
    }

    /// <summary>
    /// Custom data grid to control when collection refreshes occur
    /// </summary>
    public class CustomDataGridView : DataGridView
    {
        protected override DataGridViewRowCollection CreateRowsInstance()
        {
            return new CustomDataGridViewRowCollection(this);
        }

        //Implemented as flags to allow for other overrides to be investigated
        [Flags]
        enum States
        {
            None = 0,
            RowCollectionDirty = 1,
        }
        States _state;

        protected override void OnRowStateChanged(int rowIndex, DataGridViewRowStateChangedEventArgs e)
        {
            //Experimental: suppresses the numerous Displayed events which seem not be needed if doing manual Refreshes
            if (e.StateChanged != DataGridViewElementStates.Displayed)
            {
                base.OnRowStateChanged(rowIndex, e);
            }
        }

        internal void OnRowCollectionDirty()
        {
            //Called by row collection when it is suppressing its collection reset event
            _state |= States.RowCollectionDirty;
        }

        /// <summary>
        /// Perform any pending actions, returning false if invalidation is still needed
        /// </summary>
        public new bool Refresh()
        {
            if (_state.HasFlag(States.RowCollectionDirty))
            {
                ((CustomDataGridViewRowCollection)Rows).Refresh();
                _state &= ~States.RowCollectionDirty;
                return true;
            }
            //Still need redraw
            return false;
        }
    }
}
